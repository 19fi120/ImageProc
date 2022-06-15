#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

void readImage_arg(char *name, cv::Mat &src_img)
{
    src_img = cv::imread(name); //画像の読み込み

    if (src_img.empty()) //画像の読み込みに失敗したら終了
    {
        cout << "画像ファイルを読み込めませんでした．" << endl;
        return;
    }
}

void meanShift(cv::Mat &src_img, cv::Mat &dst_img, double hs, double hr)
{
    if (src_img.empty())
        return;

    if (src_img.type() != CV_8UC3)
        CV_Error(cv::Error::StsUnsupportedFormat, "8bit, 3chの画像を入力してください");

    cv::Size size = src_img.size();

    int centerX = 0;
    int centerY = 0;
    int count = 0;
    double epsilon = 0.0;

    //入力画像の全画素を走査する
    for (int y0 = 0; y0 < size.height; y0++)
    {
        centerY = y0;

        uchar *srcP = src_img.ptr(centerY);
        uchar *dstP = dst_img.ptr(y0);
        for (int x0 = 0; x0 < size.width; x0++)
        {
            centerX = x0;

            while (true)
            {
                int gravDistance = 255 * 255 * 255;

                //探索窓内の生成
                //centerX, centerYを中心とした一辺が2*hsの正方形
                int minx = cvRound(centerX - hs);
                minx = MAX(minx, 0);
                int miny = cvRound(centerY - hs);
                miny = MAX(miny, 0);
                int maxx = cvRound(centerX + hs);
                maxx = MIN(maxx, size.width - 1);
                int maxy = cvRound(centerY + hs);
                maxy = MIN(maxy, size.height - 1);

                uchar *centerP = src_img.ptr(centerY);
                int centerB = centerP[centerX * 3];
                int centerG = centerP[centerX * 3 + 1];
                int centerR = centerP[centerX * 3 + 2];

                //加重平均の分子(3要素)
                double moleculeB = 0;
                double moleculeG = 0;
                double moleculeR = 0;
                //加重平均の分母
                double denominator = 0;

                //探索窓内の全画素を走査し, RGB値の重心をもとめる
                for (int y1 = miny; y1 <= maxy; y1++)
                {
                    uchar *p = src_img.ptr(y1);
                    for (int x1 = minx; x1 <= maxx; x1++)
                    {
                        //探索窓の中心画素と走査中の注目画素の差
                        int b0 = centerB - p[x1 * 3];
                        int g0 = centerG - p[x1 * 3 + 1];
                        int r0 = centerR - p[x1 * 3 + 2];

                        //RGB空間のユークリッド距離を求める
                        double distance = sqrt(b0 * b0 + g0 * g0 + r0 * r0);
                        //エパネックニコフカーネルtを算出(教科書p.206下図)
                        double epanech = (abs(distance / hr) * abs(distance / hr));

                        //k(t) = 1-t(0 <= t <= 1), 0(t > 1)
                        //g(t) = -k'(t)(教科書p.207[4]②)なので
                        if (epanech <= 1)
                        {
                            moleculeB += p[x1 * 3];
                            moleculeG += p[x1 * 3 + 1];
                            moleculeR += p[x1 * 3 + 2];

                            denominator++;
                        }
                    }
                }
                double aveB = 0;
                double aveG = 0;
                double aveR = 0;

                //三要素ごとの探索窓内の重心を求める
                if (denominator > 0)
                {
                    aveB = (moleculeB / denominator);
                    aveG = (moleculeG / denominator);
                    aveR = (moleculeR / denominator);
                }

                int cgX = 0;
                int cgY = 0;
                int cgB = 0;
                int cgG = 0;
                int cgR = 0;

                //重心となるRGB値に最も近似した画素値を持つ画素を走査して求める
                for (int y2 = miny; y2 <= maxy; y2++)
                {
                    uchar *p2 = src_img.ptr(y2);
                    for (int x2 = minx; x2 < maxx; x2++)
                    {
                        //重心のRGB値と走査中の注目画素との差を求める
                        double comB = p2[x2 * 3] - aveB;
                        double comG = p2[x2 + 3 + 1] - aveG;
                        double comR = p2[x2 * 3 + 2] - aveR;
                        double comDistance = sqrt(comB * comB + comG * comG + comR * comR);

                        //ユークリッド距離が前の値よりも小さかったらその画素を重心として更新
                        //前の値となるgravDistanceの初期化はwhile文の直下
                        if (comDistance <= gravDistance)
                        {
                            cgX = x2;
                            cgY = y2;
                            cgB = p2[x2 * 3];
                            cgG = p2[x2 * 3 + 1];
                            cgR = p2[x2 * 3 + 2];

                            gravDistance = comDistance;
                        }
                    }
                }

                //重心の位置と初期の注目画素の位置が同じならば極大値として出力画素に書き込む
                if (cgX == centerX && cgY == centerY)
                {
                    dstP[x0 * 3] = cgB;
                    dstP[x0 * 3 + 1] = cgG;
                    dstP[x0 * 3 + 2] = cgR;
                    centerX = x0;
                    centerY = y0;
                    count = 0;
                    break;
                }
                // |x - 重心x| + |y - 重心y| + (b - 重心b)^2 + (g - 重心g)^2 + (r - 重心r)^2が
                //要求精度(epsilon)以下ならば極大値として出力画素に書き込む
                //(OpenCVのpyrMeanShiftFilteringから参照)
                else if (abs(centerX - cgX) + abs(centerY - cgY) + pow(centerB - cgB, 2.0) + pow(centerG - cgG, 2.0) + pow(centerR - cgR, 2.0) <= epsilon)
                {
                    dstP[x0 * 3] = cgB;
                    dstP[x0 * 3 + 1] = cgG;
                    dstP[x0 * 3 + 2] = cgR;
                    centerX = x0;
                    centerY = y0;
                    count = 0;
                    break;
                }
                //全画素走査しきった場合極大値として出力画素に書き込む
                else if (y0 == size.height - 1 && x0 == size.width - 1)
                {
                    dstP[x0 * 3] = cgB;
                    dstP[x0 * 3 + 1] = cgG;
                    dstP[x0 * 3 + 2] = cgR;
                    centerX = x0;
                    centerY = y0;
                    count = 0;
                    break;
                }
                //上記の3条件に当てはまらなかった場合は注目画素を重心画素の位置と画素値に置き換える
                else
                {
                    centerX = cgX;
                    centerY = cgY;
                    centerB = cgB;
                    centerG = cgG;
                    centerR = cgR;
                    count++;
                }

                //画素によっては要求精度が同じ値で無限ループするためカウンターにより制御
                if (count > 20)
                {
                    dstP[x0 * 3] = cgB;
                    dstP[x0 * 3 + 1] = cgG;
                    dstP[x0 * 3 + 2] = cgR;
                    centerX = x0;
                    centerY = y0;
                    count = 0;
                    break;
                }
                //注目画素に対する出力画素の画素値が求まった場合はwhile文から脱出し, 次の注目画素へと移動する
                //求まらなかった場合は重心画素の位置を中心とした新たな探索窓を生成し, 極大値が求まるまでwhile文内の処理を繰り返す
            }
        }
    }
}

int main(int argc, char *argv[])
{
    cv::Mat src_img;
    int hs = 3;               // default hs : (x, y)のバンド幅
    int hr = 3;               // default hr : (B, G, R)のバンド幅
    
    //exeファイル名, 入力画像, (x, y)のバンド幅hs, (B, G, R)のバンド幅hr
    if (argc < 4)
    {
        readImage_arg("sample.jpg", src_img); //引数省略時のdefault設定
    }
    else
    {
        readImage_arg(argv[1], src_img);
        hs = atoi(argv[2]);
        hr = atoi(argv[3]);
        cout << hs << endl;
    }

    std::string result_win_title = "MS "+ to_string(hs) + ", " + to_string(hr);

    cv::imshow("入力", src_img);

    cv::Mat dst_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3);

    meanShift(src_img, dst_img, hs, hr);
    cv::imshow(result_win_title, dst_img);

    cv::waitKey();
    return 0;
}