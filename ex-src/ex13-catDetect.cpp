#include <opencv2/opencv.hpp>

using namespace std;

int main(int argc, char *argv[])
{
	cv::Mat input;
	if (argc < 2)
		input = cv::imread("sample.jpg"); //引数省略時のdefault設定
	else
		input = cv::imread(argv[1]); //画像の読み込み

	if (input.empty()) //画像の読み込みに失敗したら終了
	{
		cout << "画像ファイルを読み込めませんでした．" << endl;
		return -1;
	}

	cv::Mat gray, smallimg, image;

	image = input.clone();

	// 分類器の読み込み
	string cascadeName_Cat = "./cascade_cat.xml"; // Cat
	cv::CascadeClassifier cascadeCat;
	if (!cascadeCat.load(cascadeName_Cat))
	{
		cout << "猫分類器が見つかりませんでした" << endl;
		return -1;
	}

	vector<cv::Rect> catFaces; //猫の顔検出領域を格納する配列
	double scale = 2.0;		   // 処理時間短縮のために画像縮小率]
	int eyemask = 0;
	int facerect = 1;

	int key = -1;
	cout << "r : 顔に枠表示，m : 目隠し，d : 顔枠,目隠し非表示，U/D：スケールUP/DOWN、q : 終了" << endl;

	while (true)
	{

		// やり直しを可能にするために，毎回入力画像をコピーする
		image = input.clone();
		cv::cvtColor(image, gray, cv::ColorConversionCodes::COLOR_BGR2GRAY); // equalizeHist()のために、グレースケール画像へ変換
																			 //		cv::equalizeHist(gray, gray); 			// ヒストグラム均一化

		// 画像サイズを変更調整し，検出対象のサイズの範囲を変更する
		cv::resize(gray, smallimg, cv::Size(image.cols / scale, image.rows / scale));

		// ここまでで下準備終わり

		//分類器によって検出する
		cascadeCat.detectMultiScale(smallimg, catFaces); // 引数：画像，出力矩形（検出された顔を囲む）

		// 結果の描画
		for (int i = 0; i < catFaces.size(); i++)
		{ //検出した顔の数だけループ
			// 元の画像サイズに合わせるために、scaleをかける
			int facex = catFaces[i].x * scale,
				facey = catFaces[i].y * scale,
				facew = catFaces[i].width * scale,
				faceh = catFaces[i].height * scale;

			if (facerect == 1)
			{ // 検出された顔を赤枠で囲む
				cv::rectangle(image, cv::Point(facex, facey), cv::Point(facex + facew, facey + faceh),
							  cv::Scalar(0, 0, 255), 3, 4);
			}
			if (eyemask == 1)
			{ // 目隠しする
				// おそらく目だろう位置のROIを抽出
				cv::Mat eye(image,
							cv::Rect(facex+facew*0.15, facey + faceh * 0.5, facew*0.7, faceh * 0.25));
				eye = cv::Scalar(50, 50, 50); // 灰色で塗りつぶす
			}
		}

		cv::imshow("Output", image); //画像の表示

		switch (cv::waitKey(33))
		{
		case 'r':
			facerect = 1; // 検出顔に赤枠
			cout << "r : 顔枠表示" << endl;
			break;
		case 'd':
			facerect = 0;
			eyemask = 0;
			cout << "d : 顔枠,目隠し非表示" << endl;
			break;
		case 'm':
			eyemask = 1; // 目隠しモード
			cout << "m : 目隠し" << endl;
			break;
		case 'U':
			if (scale <= 10.0)
			{
				scale *= 1.1;
				cout << "scale = " << scale << endl;
			}
			break;
		case 'D':
			if (scale >= 0.6)
			{
				scale *= 0.9;
				cout << "scale = " << scale << endl;
			}
			break;
		case 's':
			cv::imwrite("../image/saved.jpg", image);
			cout << "saved" << endl;
			break;
		case 'w':
			cv::imwrite("../image/saved_another.jpg", image);
			cout << "saved_another" << endl;
			break;
		case 'q':
			return 0;
		}
	}
}
