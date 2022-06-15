// From YusukeSuzuki/opencv_sample_list_jp (2018.8.30)
// https://github.com/YusukeSuzuki/opencv_sample_list_jp/blob/master/cpp_samples/dft_idft/main.cpp
// Modified to mask the power spectrum with another gray scale or binary mask image
// Usage: arg[1] = original image (grayscale or color)
//        arg[2] = gray scale mask image (thresholded with 128) for power spectrum 
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

/**
 * 象限を入替える(画像の各四半分を入れ替える)
 * +----+----+    +----+----+
 * |    |    |    |    |    |
 * | q0 | q1 |    | q3 | q2 |
 * |    |    |    |    |    |
 * +----+----+ -> +----+----+
 * |    |    |    |    |    |
 * | q2 | q3 |    | q1 | q0 |
 * |    |    |    |    |    |
 * +----+----+    +----+----+
 */
void swap_quadrant(cv::Mat& in){
	/**
	 * 以下に続く入れ替えのために、画像の両辺のサイズを偶数にしておく
	 */
	in = in( cv::Rect(0, 0, in.cols & -2, in.rows & -2) );

	const int half_width = in.cols / 2;
	const int half_height = in.rows / 2;

	cv::Mat tmp;

	cv::Mat q0(in,
		cv::Rect(0,0,half_width,half_height));
	cv::Mat q1(in,
		cv::Rect(half_width,0,half_width,half_height));
	cv::Mat q2(in,
		cv::Rect(0,half_height,half_width,half_height));
	cv::Mat q3(in,
		cv::Rect(half_width,half_height,half_width,half_height));

	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

// DFTのための複素画像を作成する
void create_complex_dft_image(const cv::Mat& in, cv::Mat& out)
{
	cv::Mat real_image; // 複素画像の実部

	/**
	 * DFTを行うには画像の両辺それぞれがDFTに適したサイズであることが必要。
	 * 入力画像をDFTに適したサイズに拡大してコピーした上であまりを塗りつぶす。
	 *
	 * cv::getOptimalDFTSize()はDFT入力された大きさに対してDFTに適した大きさを返す。
	 *
	 * cv::copyMakeBorder()は入力画像の周囲を指定幅分だけ塗りつぶして出力画像に
	 * コピーする。
	 *
	 * 今回は、DFTのサイズに拡大する分だけ右端と下端を0で塗りつぶしている。
	 */
	cv::copyMakeBorder(in, real_image,
		0, cv::getOptimalDFTSize(in.rows) - in.rows,
		0, cv::getOptimalDFTSize(in.cols) - in.cols,
		cv::BORDER_CONSTANT, cv::Scalar::all(0) );

	// 0で埋めた虚部とマージして複素画像にする。
	cv::Mat planes[] = {cv::Mat_<float>(real_image), cv::Mat::zeros(real_image.size(), CV_32F)};
	cv::merge(planes, 2, out);
}

// DFTの結果である複素画像を表示用の強度画像に変換する。
void create_fourier_magnitude_image_from_complex(const cv::Mat& in, cv::Mat& out)
{
	/**
	 * 複素画像の実部と虚部を2枚の画像に分離する。
	 */
	cv::Mat planes[2];
	cv::split(in, planes);

	/**
	 * cv::magnitude()で二枚の画像から強度を算出する。
	 *
	 * 強度画像の各ピクセル値の擬似コード
	 * dst(x,y) = sqrt(pow( src1(x,y),2) + pow(src2(x,y),2) )
	 */
	cv::magnitude(planes[0], planes[1], out);

	/**
	 * 表示用に対数に変換する。そのため各ピクセルに1を加算しておく。
	 */
	out += cv::Scalar::all(1);
	cv::log(out, out);

	// 第1,2,3,4象限 → 第3,4,1,2象限に入れ替え
	swap_quadrant(out);
	
	// 濃淡を強調するために、画像最小値を0に、最大値を1にするように正規化する。
	cv::normalize(out, out, 0, 1, cv::NORM_MINMAX);
}


// 逆DFTで得られた画像を実画像に変換する。
void create_inverse_fourier_image_from_complex(
	const cv::Mat& in, const::cv::Mat& origin, cv::Mat& out)
{
	// 複素画像の実部と虚部を2枚の画像に分離する。
	cv::Mat splitted_image[2];
	cv::split(in, splitted_image);

	/**
	 * 実部について正規化を行う。
	 * 入力画像のサイズはDFT用に拡大されているので、原画像の同サイズにROIを設定して
	 * 縮小する。
	 */
	splitted_image[0](cv::Rect(0, 0, origin.cols, origin.rows)).copyTo(out);
	cv::normalize(out, out, 0, 1, cv::NORM_MINMAX);
}

int main(int argc, char* argv[])
{
	if(argc < 2){
		cout << "usage: " << argv[0] << " image" << endl;
		return 0;
	}
	// 画像の読み込み
	cv::Mat image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
	if( image.empty() ){
		cout << "can't read " << argv[1] << endl;
		return -1;
	}
	// マスク画像の読み込み
	cv::Mat mask;
	mask.create(cv::Size(image.cols, image.rows), CV_8UC1);
	mask = cv::Scalar::all(255);
	if(argc >= 3){
		mask = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);
		if( mask.empty() ){
			cout << "can't read " << argv[2] << endl;
			return -1;
		}
	}
	// フーリエ変換のために適切なサイズの複素画像を作る
	cv::Mat complex_image;
	create_complex_dft_image(image, complex_image);

	/**
	 * フーリエ変換
	 *
	 * 複素画像から複素画像へ変換する。ここでは入力画像自体を出力先にしている。
	 *
	 * 呼び出し時のオプションにより複素画像から実画像への変換などもできる。
	 */
	cv::dft(complex_image, complex_image);

	// フーリエ変換の結果の可視化(第1,2,3,4象限→(入替)→第3,4,1,2象限)
	cv::Mat magnitude_image;
	create_fourier_magnitude_image_from_complex(complex_image, magnitude_image);

// *** マスク処理(begin) ***

	// 入力画像にマスク画像をサイズ合わせ
	cv:resize(mask, mask, image.size());

	// 入力画像にマスク画像を2値化
	// 	(128以上を255に，それ以外を0)
	// void cv::Threshold(InputArray src, OutputArray dst, 
	//			double threshold, double maxval, int type)
	cv::threshold(mask, mask, 127, 255, cv::THRESH_BINARY);

	// マスク画像をDFT用に0パディング
	cv::Mat padded_mask;
	cv::copyMakeBorder(mask, padded_mask,
			0, cv::getOptimalDFTSize(mask.rows) - mask.rows,
			0, cv::getOptimalDFTSize(mask.cols) - mask.cols,
			cv::BORDER_CONSTANT, cv::Scalar::all(0) );

	// 第1,2,3,4象限 → 第3,4,1,2象限に入れ替え
	swap_quadrant(padded_mask);
	
	/**
	 * 複素画像の実部と虚部を2枚の画像に分離し，それぞれに
	 * マスクをかけて統合し直す
	 */
	cv::Mat after_planes[2];
	split(complex_image, after_planes);

	// 複素画像の実部と虚部(それぞれCV_32FC1)にマスク(乗算)するため，
	// 2値(0,255)画像のCV_8UC1の padded_mask を
	// CV_32FC1 の2値(0.0, 1.0)画像 maskF に変換 
	cv::Mat maskF;
	padded_mask.convertTo(maskF, CV_32F, 1.0/255);
	after_planes[0] = after_planes[0].mul(maskF);
	after_planes[1] = after_planes[1].mul(maskF);
	merge(after_planes, 2, complex_image);

	// フーリエ変換の結果の可視化(第1,2,3,4象限→(入替)→第3,4,1,2象限)
	cv::Mat masked_magnitude_image;
	create_fourier_magnitude_image_from_complex(complex_image, masked_magnitude_image);

// *** マスク処理(end) ***

	/**
	 * 逆フーリエ変換
	 *
	 * 複素画像から複素画像へ変換する。ここでは入力画像自体を出力先にしている。
	 *
	 * 呼び出し時のオプションにより複素画像から実画像への変換などもできる。
	 */
	cv::idft(complex_image, complex_image);

	// 逆フーリエ変換の結果の可視化
	cv::Mat idft_image;
	create_inverse_fourier_image_from_complex(complex_image, image, idft_image);

	// 結果表示
	cv::namedWindow("original");
	cv::imshow("original", image);

	cv::namedWindow("dft");
	cv::imshow("dft", magnitude_image);

	cv::namedWindow("masked dft");
	cv::imshow("masked dft", masked_magnitude_image);

	cv::namedWindow("idft");
	cv::imshow("idft", idft_image);

	cv::waitKey();

	/**
	 * Option: 結果画像の保存
	 *
	 * 表示画像は浮動小数表現で値域が[0,1]なので255を掛けたものを入力とする。
	cv::imwrite(string(argv[1]) + ".dft.jpg", magnitude_image * 255);
	cv::imwrite(string(argv[1]) + ".dftMasked.jpg", masked_magnitude_image * 255);
	cv::imwrite(string(argv[1]) + ".idftMasked.jpg", idft_image * 255);
	 */

	return 0;
}