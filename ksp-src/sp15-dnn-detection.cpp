/*
 * Program name: sp15-dnn-detection
 * Contributor:  Akira Sekizawa, Katsuto Nakajima
 * Created date: 11/08/2018
 * Summary:      OpenCV 3.4.3でYOLOv3とMask R-CNNの検出・識別を行うプログラム
 * 
 * Debugモードでビルドすると実行時間が数倍に遅くなるため，Releaseモードでのビルドを推奨する
 * 
 * コマンドライン引数の説明:
 * 
 *     必ず指定する必要がある引数
 *     --net         yolov3またはmask_rcnnのどちらかを指定する
 *     --config      ネットワークの構成を記述したファイルのパス
 *     --model       学習済みモデルのパス
 *     --classes     クラス名を1行ずつ記述したファイルのパス．学習時に使用されたファイルと同じファイルを指定する必要がある
 *     --scale       学習時に使用された値を調べて指定する（入力画像の全画素値をscale倍する）．誤った値を指定しても認識は可能だが精度が低下する
 *     --mean        学習時に使用された値を調べて指定する（入力画像を全画素値からmeanを引く）．誤った値を指定しても認識は可能だが精度が低下する
 *     --swapChannel 学習時に使用された値を調べて指定する．入力画像のチャンネルをBGRからRGB（またはRGBからBGR）へと変換する場合に1を指定する．
 *     --type        入力ソースが画像ファイル，動画ファイル，カメラのいずれかを指定する．それぞれimage, video, cameraと指定する
 *     --input       入力ソースが画像ファイル，動画ファイルならばファイルへのパスを指定する．カメラならカメラIDを整数で指定する
 * 
 *     指定しなくてもよい引数
 *     --confThreshold 検出物体の確信度がこの値より高いものだけを検出結果として採用する
 *     --nmsThreshold  Non-Maximum Suppression処理（2つの検出物体が非常に近い位置にあるときに片方を破棄する処理）で用いる閾値
 *     --maskThreshold 検出物体のマスクの各画素の確信度がこの値より高い画素だけをマスクとして用いる
 *     --width         YOLOv3でのみ用いる引数．YOLOv3では32の倍数を指定でき，widthとheightは同じ値にする必要がある．値の大きさに比例して精度と認識時間が増加する
 *     --height        YOLOv3でのみ用いる引数．YOLOv3では32の倍数を指定でき，widthとheightは同じ値にする必要がある．値の大きさに比例して精度と認識時間が増加する
 * 
 * 実行コマンドの例
 * 
 *     画像ファイル： Mask R-CNN
 *     sp15-dnn-detection --net=mask_rcnn --config=../model/mask_rcnn_inception_v2.pbtxt --model=../model/mask_rcnn_inception_v2.pb --classes=../model/mask_rcnn_classes.txt --scale=1.0 --mean="0 0 0" --type=image --input=../image/person.png
 *     画像ファイル： YOLOv3（Tiny版），640×640（32の倍数ならば任意の解像度を指定できる）
 *     sp15-dnn-detection --net=yolov3 --config=../model/yolov3-tiny.cfg --model=../model/yolov3-tiny.weights --classes=../model/yolov3_classes.txt --scale=0.00392157 --mean="0 0 0" --width=640 --height=640 --type=image --input=../image/person.png
 *     画像ファイル： YOLOv3（Tiny版），1280×1280（32の倍数ならば任意の解像度を指定できる）
 *     sp15-dnn-detection --net=yolov3 --config=../model/yolov3-tiny.cfg --model=../model/yolov3-tiny.weights --classes=../model/yolov3_classes.txt --scale=0.00392157 --mean="0 0 0" --width=1280 --height=1280 --type=image --input=../image/person.png
 *     カメラ： Mask R-CNN
 *     sp15-dnn-detection --net=mask_rcnn --config=../model/mask_rcnn_inception_v2.pbtxt --model=../model/mask_rcnn_inception_v2.pb --classes=../model/mask_rcnn_classes.txt --scale=1.0 --mean="0 0 0" --type=camera --input=0
 *     カメラ： YOLOv3（Tiny版），640×640（32の倍数ならば任意の解像度を指定できる）
 *     sp15-dnn-detection --net=yolov3 --config=../model/yolov3-tiny.cfg --model=../model/yolov3-tiny.weights --classes=../model/yolov3_classes.txt --scale=0.00392157 --mean="0 0 0" --width=640 --height=640 --type=camera --input=0
 *     カメラ： YOLOv3（Tiny版），1280×1280（32の倍数ならば任意の解像度を指定できる）
 *     sp15-dnn-detection --net=yolov3 --config=../model/yolov3-tiny.cfg --model=../model/yolov3-tiny.weights --classes=../model/yolov3_classes.txt --scale=0.00392157 --mean="0 0 0" --width=1280 --height=1280 --type=camera --input=0
 * 
 * プログラムの終了方法
 * 
 *     方法1: 認識結果の表示ウィンドウ上で何かキーを押す
 *     方法2: コマンドプロンプトで Ctrl + C を押す
 * 
 * Changes
 * 11/08/2018: YOLOv3とMask R-CNNのソースコードを1つにまとめた
 * 11/14/2018: Mask R-CNNのマスクの描画色を「画面左上のオブジェクトから順番に，色相値に70を加算しながらHSV = (x, 255, 255)で塗りつぶしていく」アルゴリズムに変更した
 */

#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;

/*
 * コマンドライン引数をパースし，コマンドライン引数と値のペアを格納したオブジェクトを返す関数
 * 
 * 引数:
 *     argc (int): 引数の個数
 *     argv (char**): 引数の実体を格納した2次元配列
 * 返り値:
 *     cv::CommandLineParser: コマンドライン引数と値のペアを格納したオブジェクト
 */
cv::CommandLineParser parseArgs(int argc, char **argv)
{
    const char *options =
        "{ help  h       |       | Print help message. }"
        "{ net           |       | Choose model type: yolov3, mask_rcnn }"
        "{ type          |       | Choose input type: image, video, camera. }"
        "{ input         |       | Path to input file or camera number. }"
        "{ config        |       | Path to a network configuration file. }"
        "{ model         |       | Path to a trained model. }"
        "{ classes       |       | Path to a label lists. }"
        "{ confThreshold |  0.2  | [Optional] Threshold of object classification confidences. }"
        "{ nmsThreshold  |  0.4  | [Optional] Threshold of Non-Maximum Suppression algorithm. }"
        "{ maskThreshold |  0.3  | [Optional] Threshold of mask confidences. }"
        "{ width         |   0   | [Optional] Width of input images for the network.}"
        "{ height        |   0   | [Optional] Height of input images for the network.}"
        "{ scale         |       | Multiply pixel values by 'scale' in input image. }"
        "{ mean          |       | Subtract 'mean' from pixel values in input image. }"
        "{ swapChannel   |       | Change channel of input image BGR to RGB (or RGB to BGR). }";
    cv::CommandLineParser parser(argc, argv, options);

    // 引数が指定されずに実行された場合はヘルプを表示して終了する
    if (argc == 1 || parser.has("help"))
    {
        parser.printMessage();
        exit(-1);
    }

    return parser;
}

/*
 * コマンドライン引数で指定された入力ソースをcv::VideoCaptureにセットして返す関数
 * 
 * 引数:
 *     parser (cv::CommandLineParser): コマンドライン引数と値のペアを格納したオブジェクト
 * 返り値:
 *     cv::VideoCapture: 画像，動画，カメラのいずれかを入力ソースにセットした cv::VideoCaptureオブジェクト
 */
cv::VideoCapture getInputSource(cv::CommandLineParser &parser)
{
    cv::VideoCapture inputSource;

    if (parser.get<cv::String>("type") == "image")
        // 画像ファイルが指定された場合
        inputSource = cv::VideoCapture(parser.get<cv::String>("input"));
    else if (parser.get<cv::String>("type") == "video")
        // 動画ファイルが指定された場合
        inputSource = cv::VideoCapture(parser.get<cv::String>("input"));
    else if (parser.get<cv::String>("type") == "camera")
        // カメラが指定された場合
        inputSource = cv::VideoCapture(parser.get<int>("input"));
    else
    {
        // 上記以外の入力ソースが指定された場合
        cerr << "Unknown input type was specified: " << parser.get<cv::String>("type");
        exit(-1);
    }

    return inputSource;
}

/*
 * ファイルを読み込んでベクターとして返す関数
 * 
 * 引数:
 *     fileName (string): ファイル名
 * 返り値:
 *     vector<string>: ファイルの各行を格納したベクター
 */
vector<string> readLines(string fileName)
{
    vector<string> lines;
    ifstream ifs(fileName);
    string line;
    while (getline(ifs, line))
        lines.push_back(line);
    return lines;
}

/*
 * 学習済みモデルを読み込む関数
 * 
 * 引数:
 *     model (cv::String): ネットワークの学習済みモデルの名前
 *     config (cv::String): ネットワーク構成を記述したファイルの名前
 *     backendId (int): ネットワークの実行時に用いるバックエンドを指定する
 *     targetId (int): 実行対象のハードウェア（CPU，GPU）を指定する
 * 返り値:
 *     cv::dnn::Net: 学習済みモデルを読み込んだネットワークオブジェクト
 */
cv::dnn::Net initializeNetwork(const cv::String &model, const cv::String &config, int backendId, int targetId)
{
    cv::dnn::Net net = cv::dnn::readNet(model, config);
    net.setPreferableBackend(backendId);
    net.setPreferableTarget(targetId);
    return net;
}

/*
 * ネットワークを実行し，指定した層の出力値を得る関数
 * 
 * 引数:
 *     net (cv::dnn::Net): 学習済みモデルを読み込んだネットワークオブジェクト
 *     names (vector<cv::String>): ネットワーク内のどの層の出力値を得るか指定する
 * 返り値:
 *     vector<cv::Mat>: 指定された層の出力値（cv::Mat）を格納したベクター
 */
vector<cv::Mat> runNetwork(cv::dnn::Net &net, vector<cv::String> names)
{
    vector<cv::Mat> outputs;
    net.forward(outputs, names);
    return outputs;
}

/*
 * Mask R-CNNの出力値として用いるネットワークの層の名前を返す関数
 * 
 * 返り値:
 *     vector<cv::String>: 層の名前を格納したベクター
 */
vector<cv::String> maskRcnnOutputNames()
{
    vector<cv::String> names = {"detection_out_final", "detection_masks"};
    return names;
}

/*
 * YOLOv3の出力値として用いるネットワークの層の名前を返す関数
 * 
 * 引数:
 *     net (cv::dnn::Net): YOLOv3のネットワーク
 * 返り値:
 *     vector<cv::String>: 層の名前を格納したベクター
 */
vector<cv::String> yoloV3OutputNames(cv::dnn::Net &net)
{
    vector<cv::String> names;
    for (int index : net.getUnconnectedOutLayers())
        names.push_back(net.getLayerNames()[index - 1]);
    return names;
}

/*
 * ネットワークの出力値をMask R-CNNの出力値として解釈する関数
 * 
 * 引数:
 *     output (vector<cv::Mat>): ネットワークの出力値を格納したベクター
 *     imageSize (cv::Size): 入力画像のサイズ
 *     confThreshold (float): 検出結果の確信度の閾値
 *     maskThreshold (float): 生成マスクの各画素の確信度の閾値
 *     classIds (vector<int>): 検出物体のクラスID
 *     confidences (vector<float>): 検出物体の確信度
 *     boxes (vector<cv::Rect>): 検出物体の矩形領域
 *     masks (vector<cv::Mat>): 検出物体のマスク
 */
void parseResultAsMaskRCNN(const vector<cv::Mat> &output,
                           const cv::Size imageSize,
                           const float confThreshold,
                           const float maskThreshold,
                           vector<int> &classIds,
                           vector<float> &confidences,
                           vector<cv::Rect> &boxes,
                           vector<cv::Mat> &masks)
{
    // 検出結果を格納したcv::Mat
    cv::Mat detectionResult = output[0];

    // マスク生成結果を格納したcv::Mat
    cv::Mat maskResult = output[1];

    // ネットワークで生成される物体マスクのサイズ（このサイズは学習時に固定される）
    cv::Size maskSize(maskResult.size[2], maskResult.size[3]);

    // Mask R-CNNの出力結果は4次元配列を格納したcv::Matであるため3重ループを用いる
    for (int i0 = 0; i0 < detectionResult.size[0]; i0++)
        for (int i1 = 0; i1 < detectionResult.size[1]; i1++)
            for (int i2 = 0; i2 < detectionResult.size[2]; i2++)
            {
                // 4次元配列の各要素にアクセスするには配列のインデックスをint型配列として渡す必要がある
                int indices[][4] = {
                    {i0, i1, i2, 0},
                    {i0, i1, i2, 1},
                    {i0, i1, i2, 2},
                    {i0, i1, i2, 3},
                    {i0, i1, i2, 4},
                    {i0, i1, i2, 5},
                    {i0, i1, i2, 6},
                };

                // ネットワークの出力データから必要な値を取得する
                float classId = detectionResult.at<float>(indices[1]); // クラスID（0, 1, ..., クラス数）
                float score = detectionResult.at<float>(indices[2]);   // 確信度（0.0～1,0）
                float rawX0 = detectionResult.at<float>(indices[3]);   // x0の相対座標（0.0～1,0）
                float rawY0 = detectionResult.at<float>(indices[4]);   // y0の相対座標（0.0～1,0）
                float rawX1 = detectionResult.at<float>(indices[5]);   // x1の相対座標（0.0～1,0）
                float rawY1 = detectionResult.at<float>(indices[6]);   // y1の相対座標（0.0～1,0）

                // 検出結果の確信度が閾値より低い場合はスキップする
                if (score <= confThreshold)
                    continue;

                // 相対座標を絶対座標へと変換する
                int x0 = imageSize.width * rawX0;
                int y0 = imageSize.height * rawY0;
                int x1 = imageSize.width * rawX1;
                int y1 = imageSize.height * rawY1;

                // 0未満や，幅や高さを超える座標を範囲内に収める
                x0 = max(0, min(x0, imageSize.width - 1));
                y0 = max(0, min(y0, imageSize.height - 1));
                x1 = max(0, min(x1, imageSize.width - 1));
                y1 = max(0, min(y1, imageSize.height - 1));
                cv::Rect box = cv::Rect(cv::Point(x0, y0), cv::Point(x1, y1));

                // 物体を囲むマスクの先頭画素のポインタを得る
                float *pointerToMask = maskResult.ptr<float>(i2, (int)classId);

                // ポインタをcv::Matに変換する
                cv::Mat mask(maskSize, CV_32FC1, pointerToMask);

                // マスクは物体サイズに関わらず固定サイズで生成されるので，物体の矩形に合わせてリサイズする
                cv::resize(mask, mask, cv::Size(box.width, box.height));

                // マスクの各画素はそれ自体が確信度を表すので，マスクの確信度で閾値処理を行う
                mask >= maskThreshold;

                // マスクを8-bitのグレースケール画像へと変換する
                mask.convertTo(mask, CV_8UC1);

                // クラス，確信度，矩形領域，マスクをそれぞれベクタに追加する
                classIds.push_back((int)classId);
                confidences.push_back(score);
                boxes.push_back(box);
                masks.push_back(mask);
            }
}

/*
 * ネットワークの出力値をYOLOv3の出力値として解釈する関数
 * 
 * 引数:
 *     output (vector<cv::Mat>): ネットワークの出力値を格納したベクター
 *     imageSize (cv::Size): 入力画像のサイズ
 *     confThreshold (float): 検出結果の確信度の閾値
 *     classIds (vector<int>): 検出物体のクラスID
 *     confidences (vector<float>): 検出物体の確信度
 *     boxes (vector<cv::Rect>): 検出物体の矩形領域
 */
void parseResultAsYoloV3(const vector<cv::Mat> &output,
                         const cv::Size imageSize,
                         const float confThreshold,
                         vector<int> &classIds,
                         vector<float> &confidences,
                         vector<cv::Rect> &boxes)
{
    // YOLOv3の出力データは2次元配列である
    for (cv::Mat result : output)
        for (int i = 0; i < result.rows; i++)
        {
            // YOLOv3は全てのクラスの確信度を出力する
            // （80クラスで認識した場合は80個の確信度が出力される）
            // よって最初に全クラスの確信度から最大の各進度を持つクラスを求める必要がある
            cv::Mat allScores = result.row(i).colRange(5, result.cols);
            cv::Point index;
            double score;
            cv::minMaxLoc(allScores, NULL, &score, NULL, &index);
            int classId = index.x;

            // 確信度が閾値より低い場合はスキップする
            if (score <= confThreshold)
                continue;

            // ネットワークの出力データ（2次元配列）から必要な値を取得する
            float rawCenterX = result.at<float>(i, 0); // 物体の中心の相対座標（0.0～1,0）
            float rawCenterY = result.at<float>(i, 1); // 物体の中心の相対座標（0.0～1,0）
            float rawWidth = result.at<float>(i, 2);   // 物体の相対的な幅（0.0～1,0）
            float rawHeight = result.at<float>(i, 3);  // 物体の相対的な高さ（0.0～1,0）

            // 相対座標を絶対座標へと変換する
            int centerX = imageSize.width * rawCenterX;
            int centerY = imageSize.height * rawCenterY;
            int width = imageSize.width * rawWidth;
            int height = imageSize.height * rawHeight;
            int x0 = centerX - width / 2;
            int y0 = centerY - height / 2;
            int x1 = centerX + width / 2;
            int y1 = centerY + height / 2;

            // 0未満や，幅や高さを超える座標を範囲内に収める
            x0 = max(0, min(x0, imageSize.width - 1));
            y0 = max(0, min(y0, imageSize.height - 1));
            x1 = max(0, min(x1, imageSize.width - 1));
            y1 = max(0, min(y1, imageSize.height - 1));
            cv::Rect box = cv::Rect(cv::Point(x0, y0), cv::Point(x1, y1));

            // クラス，確信度，矩形領域をそれぞれベクタに追加する
            classIds.push_back((int)classId);
            confidences.push_back(score);
            boxes.push_back(box);
        }
}

/*
 * 検出物体を囲む矩形の枠線を描画する
 * 
 * 引数:
 *     dst (cv::Mat): 描画対象の画像
 *     box (cv::Rect): 検出物体の矩形座標
 *     color (cv::Scalar): 描画の色
 *     strokeWidth (int): 描画の太さ
 */
void drawBoundingBox(cv::Mat dst, cv::Rect box, cv::Scalar color, int strokeWidth)
{
    cv::rectangle(dst, box, color, strokeWidth);
}

/*
 * 検出物体を半透明なマスクで塗りつぶす関数
 * 
 * 引数:
 *     dst (cv::Mat): 描画対象の画像
 *     box (cv::Rect): 検出物体の矩形座標
 *     mask (cv::Mat): 検出物体のマスク
 *     colorBackground (cv::Scalar): 塗りつぶす色
 *     alpha (float): 塗りつぶす色の不透明度．0～1で指定し，1に近いほど濃く塗りつぶす
 */
void fillMask(cv::Mat &dst, cv::Rect box, cv::Mat mask, cv::Scalar fillColor, float alpha)
{
    cv::Mat dstRoI = dst(box);

    // 塗りつぶしたい色と元々の画像の色を aalpha : 1 - alpha で足し合わせれば半透明に塗りつぶせる
    cv::Mat rectMat = dstRoI.clone();
    rectMat *= 1 - alpha;
    rectMat += alpha * fillColor;

    rectMat.copyTo(dstRoI, mask);
}

/*
 * 物体のクラス名を表示する関数
 * 
 * 引数:
 *     dst (cv::Mat): 描画対象の画像
 *     box (cv::Rect): 検出物体の矩形座標
 *     label (string): クラス名
 *     confidence (float): 検出物体の確信度
 *     colorText (cv::Scalar): 文字色
 *     colorBackground (cv::Scalar): 背景色
 *     alphaBackground (float): 背景色の不透明度．0～1で指定し，1に近いほど濃く塗りつぶす
 *     strokeWidth (int): 描画の太さ
 */
void drawLabel(cv::Mat dst, cv::Rect box, string label, float confidence, cv::Scalar colorText, cv::Scalar colorBackground, float alphaBackground, int strokeWidth)
{
    cv::String text = cv::format("%s: %.4f", label.c_str(), confidence);

    // 表示するテキストを囲む矩形のサイズ（英語のp, q, y, jなどの下半分を除いた幅と高さ）とベースライン（英語のp, q, y, jなどの下半分の高さ）を求める
    int baseLine;
    cv::Size labelSize = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    // 描画位置を正確に計算する
    int margin = 3;
    int x0 = box.x - strokeWidth / 2;
    int y0 = box.y - labelSize.height - baseLine - strokeWidth / 2 - margin * 2;
    x0 = max(0, min(x0, dst.cols - 1));
    y0 = max(0, min(y0, dst.rows - 1));
    int x1 = x0 + labelSize.width + margin * 2;
    int y1 = y0 + labelSize.height + baseLine + margin * 2;
    x1 = max(0, min(x1, dst.cols - 1));
    y1 = max(0, min(y1, dst.rows - 1));

    // テキストを囲むボックスを半透明に塗る
    cv::Mat dstRoI = dst(cv::Rect(cv::Point(x0, y0), cv::Point(x1, y1)));
    dstRoI *= 1 - alphaBackground;
    dstRoI += alphaBackground * colorBackground;

    // ボックスの中にテキストを描画する
    cv::putText(dst, text, cv::Point(x0 + margin, y0 + labelSize.height + margin), cv::FONT_HERSHEY_SIMPLEX, 0.5, colorText, 1, cv::LineTypes::LINE_AA);
}

/*
 * 画面左上にネットワークの実行時間を表示する関数（入力層から出力層までに要した時間である．入力前のブロブへの変換や出力後の値の解釈に要した時間を含まない）
 * 
 * 引数:
 *     dst (cv::Mat): 描画対象の画像
 *     net (cv::dnn::Net): 認識に用いたネットワーク
 *     colorText (cv::Scalar): 文字色
 *     colorBackground (cv::Scalar): 背景色
 *     alphaBackground (float): 背景色の不透明度．0～1で指定し，1に近いほど濃く塗りつぶす
 */
void drawInferenceTime(cv::Mat dst, cv::dnn::Net &net, cv::Scalar colorText, cv::Scalar colorBackground, float alphaBackground)
{
    double time = 1000.0 * net.getPerfProfile(vector<double>()) / cv::getTickFrequency();
    cv::String text = cv::format("Inference time: %.2f ms", time);

    // 表示するテキストを囲む矩形のサイズ（英語のp, q, y, jなどの下半分を除いた幅と高さ）とベースライン（英語のp, q, y, jなどの下半分の高さ）を求める
    int baseLine;
    cv::Size labelSize = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    // 描画位置を正確に計算する
    int margin = 3;

    int x0 = 0;
    int y0 = 0 - margin * 2;
    x0 = max(0, min(x0, dst.cols - 1));
    y0 = max(0, min(y0, dst.rows - 1));

    int x1 = x0 + labelSize.width + margin * 2;
    int y1 = y0 + labelSize.height + baseLine + margin * 2;
    x1 = max(0, min(x1, dst.cols - 1));
    y1 = max(0, min(y1, dst.rows - 1));

    // テキストを囲むボックスを半透明に塗る
    cv::Mat dstRoI = dst(cv::Rect(cv::Point(x0, y0), cv::Point(x1, y1)));
    dstRoI *= 1 - alphaBackground;
    dstRoI += alphaBackground * colorBackground;

    // ボックスの中にテキストを描画する
    cv::putText(dst, text, cv::Point(x0 + margin, y0 + labelSize.height + margin), cv::FONT_HERSHEY_SIMPLEX, 0.5, colorText, 1, cv::LineTypes::LINE_AA);
}

/*
 * バウンディングボックスを画面左上に近い順にソートする
 * 
 * 引数:
 *     boxes (vector<cv::Rect>): バウンディングボックス
 *     indices (vector<int>): ソート後のベクターのインデックス値を格納する
 */
void sortBoxes(vector<cv::Rect> boxes, vector<int> indices)
{
    std::vector<std::pair<int, int>> distances;

    for (int i : indices)
    {
        cv::Rect box = boxes[i];
        distances.push_back(std::pair<int, int>(box.x * box.x + box.y * box.y, i));
    }

    for (int i = 0; i < indices.size(); i++)
    {
        indices[i] = distances[i].second;
    }
}

int main(int argc, char **argv)
{
    // コマンドライン引数をパースする
    cv::CommandLineParser parser = parseArgs(argc, argv);

    // ネットワークがYOLOv3かMask R-CNNか判定する
    cv::String netType = parser.get<cv::String>("net");

    // プログラムで用いる入力ソースを指定する
    cv::VideoCapture inputSource = getInputSource(parser);

    // 使用する学習済みモデルを指定する
    const cv::String config = parser.get<cv::String>("config");       // ネットワーク構成を記述したファイル
    const cv::String model = parser.get<cv::String>("model");         // ネットワークの学習済みの重み値を記述したファイル
    const cv::String classesFile = parser.get<cv::String>("classes"); // 認識対象のクラス名を記述したファイル

    // 認識時に変えられるパラメータを指定する
    const float confThreshold = parser.get<float>("confThreshold"); // 検出物体の確信度の閾値
    const float nmsThreshold = parser.get<float>("nmsThreshold");   // 検出物体同士が重なっている場合に，同一の物体の見なす際の閾値
    const float maskThreshold = parser.get<float>("maskThreshold"); // 検出物体のマスクを構成する各画素の核心殿閾値

    // 以下のパラメータにはモデルの学習時と同じ値を指定する
    const cv::Size inputSize(parser.get<int>("width"), parser.get<int>("height")); // ネットワークに与える画像のサイズ
    const float scale = parser.get<float>("scale");                                // ネットワーク入力時に画像の画素値をscale倍する
    const cv::Scalar mean = parser.get<cv::Scalar>("mean");                        // ネットワーク入力時に画像の画素値からmeanを減算する
    const bool swapChannel = parser.get<float>("swapChannel");                     // ネットワーク入力時に画像のBGRとRGBを反転する

    // クラス名をファイルから読み込む
    vector<string> classes = readLines(classesFile);

    // 学習済みモデルを読み込み，ネットワークをメモリ上に初期化する
    cv::dnn::Net net = initializeNetwork(model, config, cv::dnn::DNN_BACKEND_DEFAULT, cv::dnn::DNN_TARGET_CPU);

    cv::Mat image; // 入力画像
    cv::Mat blob;  // 入力画像をネットワークに入力可能な形式（ブロブ）に加工したオブジェクト
    while (true)
    {
        // 1フレーム分の画像を読み込む
        if (parser.get<cv::String>("type") == "image")
            image = cv::imread(parser.get<cv::String>("input"));
        else
            inputSource.read(image);

        // 入力画像をネットワークに入力可能な形式（ブロブ）に変換する
        cv::dnn::blobFromImage(image, blob, scale, inputSize, mean, swapChannel, false);

        // ブロブをネットワークの入力値としてセットする
        net.setInput(blob);

        // ネットワークを実行し，引数で指定した層の出力値（cv::Mat）を得る
        vector<cv::Mat> output;
        if (netType == "yolov3")
            output = runNetwork(net, yoloV3OutputNames(net));
        else if (netType == "mask_rcnn")
            output = runNetwork(net, maskRcnnOutputNames());
        else
        {
            cerr << "Unknown net type was specified: " << parser.get<cv::String>("net");
            exit(-1);
        }

        // ネットワークの出力値をYOLOv3またはMask R-CNNの出力値として解釈する
        vector<int> classIds;      // 検出物体のクラス名
        vector<float> confidences; // 検出物体の確信度
        vector<cv::Rect> boxes;    // 検出物体の矩形領域
        vector<cv::Mat> masks;     // 検出物体のマスク
        if (netType == "yolov3")
            parseResultAsYoloV3(output, image.size(), confThreshold, classIds, confidences, boxes);
        else if (netType == "mask_rcnn")
            parseResultAsMaskRCNN(output, image.size(), confThreshold, maskThreshold, classIds, confidences, boxes, masks);

        // Non-Maximum Suppression処理を実施する（2つの矩形領域が非常に近い位置にあるとき，片方を破棄する処理）
        vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

        // バウンディングボックスを左上に近い順にソートする
        sortBoxes(boxes, indices);

        // Mask R-CNNのマスクの描画色
        int maskFillColorHue = 0;

        // 取得したクラス，確信度，矩形領域，マスクを用いて目的の処理を行う
        for (int i : indices)
        {
            string label = classes[classIds[i]]; // 物体のクラス名
            float confidence = confidences[i];   // 確信度
            cv::Rect box = boxes[i];             // 矩形座標
            cv::Mat mask;                        // マスク
            if (netType == "mask_rcnn")
                mask = masks[i];

            /*
            * これ以降に認識結果に対して目的の処理を行うコードを記述する
            */

            // これ以降の描画の太さ
            int strokeWidth = 2;

            // 物体を囲む矩形の枠線を描画する
            drawBoundingBox(image, box, cv::Scalar(0, 0, 255), strokeWidth);

            // Mask R-CNNの場合のみ以下を実行する
            if (netType == "mask_rcnn")
            {
                // Mask R-CNNのマスクの色はHSV = (x, 255, 255)とし，xはオブジェクトごとに70を加算する
                maskFillColorHue = (maskFillColorHue + 70) % 180;
                cv::Mat hsv = cv::Mat(1, 1, CV_8UC3, cv::Scalar(maskFillColorHue, 255, 255));
                cv::Mat bgr;
                cv::cvtColor(hsv, bgr, cv::ColorConversionCodes::COLOR_HSV2BGR);

                // 生成した色で塗りつぶす
                fillMask(image, box, mask, bgr.at<cv::Vec3b>(0, 0), 0.6);
            }

            // 物体の左上にクラスを表示する
            drawLabel(image, box, label, confidence, cv::Scalar(0, 0, 0), cv::Scalar(0, 255, 0), 0.7, strokeWidth);
        }

        // 画面左上に実行時間を表示する
        drawInferenceTime(image, net, cv::Scalar(0, 0, 0), cv::Scalar(0, 255, 0), 0.7);

        // 認識結果を表示する
        cv::imshow("Result", image);

        // キー入力が行われたか，あるいは入力ソースが画像のときはループを抜ける
        if (cv::waitKey(33) != -1 || parser.get<cv::String>("type") == "image")
            break;
    }

    cv::waitKey(0);
    return 0;
}