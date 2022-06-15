/*
Author              :D.Kuwahara, modified by K.Nakajima
First Created date  :2018/10/31, modified 2018/11/06
Version             :1.1
Contents(Japanese)  :細線化処理を行うプログラム
*/
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;

/*データ型を作成*/
typedef vector<vector<uchar>> Filter;   //3*3のフィルタを格納するデータ型

/*関数プロトタイプ*/

//入力されたMatデータにおける，注目画素(x,y)の周囲3*3に存在するフィルタをvectorに入れて返す関数
inline void extractFilter(const int x, const int y, const cv::Mat &src, Filter &dst);

//入力されたフィルタ2種類が同じかどうかを比較する関数
inline bool compareFilter(const Filter &p1, const Filter &p2, const unsigned char wildNumber);

//削除される対象のピクセルであるかを確認する関数
inline bool isdeletePixel(const int x, const int y, const cv::Mat &src,
        const vector<Filter> &deletePattern,const vector<Filter> &undeletePattern);

//並列法を用いて細線化を行う関数
inline void skeleton(const cv::Mat &Image, cv::Mat &dst);

/*関数実装*/
int main(int argc, char* argv[])
{
	if(argc < 3){
        cerr << "引数は3個以上必要です．" << endl << "使用例:" << argv[0] << " input.jpg output.jpg" << endl;
		return -1;
	}
	// 画像の読み込み
	cv::Mat grayimg = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
	if( grayimg.empty() ){
		cout << "can't read " << argv[1] << endl;
		return -1;
	}
    /*念のための2値化*/
    cv::Mat binary;
    cv::threshold(grayimg, binary, 128, 255, cv::THRESH_BINARY);

    /*画像の細線化*/
    cv::Mat output;
    skeleton(binary, output);

    /*画像の表示*/
    cv::imshow("Input", grayimg);
    cv::imshow("Binary", binary);
    cv::imshow("Skelton", output);
    /*キーが押されるまで待機*/
    cv::waitKey();
    /*作成した細線化画像の保存*/
    cv::imwrite(string(argv[2]), output);
    return 0;
}

/*
入力されたMatデータにおける，注目画素(x,y)の周囲3*3に存在するフィルタをvectorに入れて返す関数
引数    x               :注目画素のx座標
引数    y               :注目画素のy座標
引数    src             :フィルタを取り出すMatデータ
引数    dst             :フィルタを格納する変数(格納順番はy,x)
*/
inline void extractFilter(const int x, const int y, const cv::Mat &src, Filter &dst){
    /*フィルタの取り出し*/
    for(int i=y-1;i<=y+1;i++){
        const unsigned char* p=src.ptr(i);
        for(int j=x-1;j<=x+1;j++){
            dst[i-y+1][j-x+1]=p[j];
        }
    }
}

/*
入力されたフィルタ2種類が同じかどうかを比較する関数
引数    p1              :１つ目の比較するフィルタ
引数    p2              :２つ目の比較するフィルタ
引数    wildNumber      :なんの数字が来ても正しいと判定される数
戻り値                  :p1,p2が同じであればtrue,そうでなければfalseを返す
*/
inline bool compareFilter(const Filter &p1, const Filter &p2, const unsigned char wildNumber){
    /*同じデータか確認*/
    for(int i=0;i<(int)p1.size();i++){
        for(int j=0;j<(int)p1[i].size();j++){
            //ワイルドナンバーでない　かつ　2つのフィルタの値が違う場合には，falseを返す
            if(p1[i][j]!=p2[i][j] && p1[i][j]!=wildNumber && p2[i][j]!=wildNumber) return false;
        }
    }
    return true;
}

/*
削除される対象のピクセルであるかを確認する
引数    x               :注目画素のx座標
引数    y               :注目画素のy座標
引数    src             :変換元の2値(0,255)画像データを格納するMat
引数    deletePattern   :削除対象となるフィルタパターン
引数    undeletePattern :削除対象となったフィルタで，削除しない場合のフィルタパターン
戻り値                  :trueは削除対象，falseは削除対象ではない
*/
inline bool isdeletePixel(const int x, const int y, const cv::Mat &src,
        const vector<Filter> &deletePattern,const vector<Filter> &undeletePattern){
    Filter filter{                //フィルタ格納用変数
        {100,100,100},
        {100,100,100},
        {100,100,100}
    };
    //注目画素からフィルタを抽出
    extractFilter(x,y,src,filter);
    //削除パターンとのマッチング
    bool match_flag=false;
    //削除する場合のフィルタとマッチするか?
    for(int i=0;i<(int)deletePattern.size();i++){
        //ワイルドナンバーを127に設定して削除する場合のフィルタとマッチするかを確認する
       if(compareFilter(filter,deletePattern[i],127)){
           match_flag=true;
       }
    }
    if(!match_flag) return false;
    //例外的に削除しないフィルタに合致していないか
    for(int i=0;i<(int)undeletePattern.size();i++){
        //ワイルドナンバーを127に設定して削除する場合のフィルタとマッチするかを確認する
        if(compareFilter(filter,undeletePattern[i],127)) return false;
    }
    return true;
}

/*
並列法を用いて細線化を行う関数
引数    src             :変換元の2値(0,255)画像データを格納するMat
引数    dst             :変換後の画像データを格納するMat
*/
inline void skeleton(const cv::Mat &src, cv::Mat &dst){
    /*引数の処理*/
    if(src.empty()){
        //引数が空の場合
        cerr << __func__ << ":引数が空です．" << endl;
    }
    /*削除する場合のフィルタを格納した定数を定義 array[Y座標][X座標]でアクセス可能*/
    const vector<Filter> deleteFilter1{
        {
            {127,255,127},
            {127,  0,127},
            {127,127,127}
        },
        {
            {127,127,127},
            {127,  0,255},
            {127,127,127}
        }

    };
    const vector<Filter> deleteFilter2{
        {
            {127,127,127},
            {127,  0,127},
            {127,255,127}
        },
        {
            {127,127,127},
            {255,  0,127},
            {127,127,127}
        }

    };

    /*削除する場合の例外のフィルタを格納した定数を定義 array[Y座標][X座標]でアクセス可能*/
    const vector<Filter> undeleteFilter1{
        {
            {127,255,127},
            {127,  0,  0},
            {127,  0,255}
        },
        {
            {255,  0,127},
            {  0,  0,255},
            {127,127,127}
        },
        {
            {127,127,127},
            {255,  0,255},
            {127,  0,127}
        },
        {
            {127,255,127},
            {  0,  0,127},
            {127,255,127}
        },
        {
            {127,  0,127},
            {255,  0,255},
            {127,127,127}
        },
        {
            {127,255,127},
            {127,  0,  0},
            {127,255,127}
        },
        {
            {127,127,127},
            {255,  0,127},
            {  0,255,127}
        },
        {
            {  0,255,127},
            {255,  0,127},
            {127,127,127}
        },
        {
            {127,255,  0},
            {127,  0,255},
            {127,127,127}
        },
        {
            {127,127,127},
            {127,  0,255},
            {127,255,  0}
        },
        {
            {255,  0,255},
            {  0,  0,  0},
            {255,127,255}
        },
        {
            {255,  0,255},
            {127,  0,  0},
            {255,  0,255}
        },
        {
            {255,127,255},
            {  0,  0,  0},
            {255,  0,255}
        },
        {
            {255,  0,255},
            {  0,  0,127},
            {255,  0,255}
        }
    };
    const vector<Filter> undeleteFilter2{
        {
            {255,  0,127},
            {  0,  0,127},
            {127,255,127}
        },
        {
            {127,127,127},
            {255,  0,  0},
            {127,  0,255}
        },
        {
            {127,127,127},
            {255,  0,255},
            {127,  0,127}
        },
        {
            {127,255,127},
            {  0,  0,127},
            {127,255,127}
        },
        {
            {127,  0,127},
            {255,  0,255},
            {127,127,127}
        },
        {
            {127,255,127},
            {127,  0,  0},
            {127,255,127}
        },
        {
            {127,127,127},
            {255,  0,127},
            {  0,255,127}
        },
        {
            {  0,255,127},
            {255,  0,127},
            {127,127,127}
        },
        {
            {127,255,  0},
            {127,  0,255},
            {127,127,127}
        },
        {
            {127,127,127},
            {127,  0,255},
            {127,255,  0}
        },
        {
            {255,  0,255},
            {  0,  0,  0},
            {255,127,255}
        },
        {
            {255,  0,255},
            {127,  0,  0},
            {255,  0,255}
        },
        {
            {255,127,255},
            {  0,  0,  0},
            {255,  0,255}
        },
        {
            {255,  0,255},
            {  0,  0,127},
            {255,  0,255}
        }
    };
    /*細線化処理*/
    cv::Mat verification;               //検証用画像
    cv::Mat renewal=src.clone();        //更新用画像
    bool end_flag=false;            //細線化処理の終了フラグが格納される変数
    while(!end_flag){
        end_flag=true;
        //更新用画像を検証用画像にコピー
        verification=renewal.clone();
        //上と右から検索する場合
        for(int y=1;y<verification.rows-1;y++){
            for(int x=verification.cols-2;x>=1;x--){
                //注目画素が白の場合には，次の画素へと移動する
                if( (verification.ptr(y))[x] != 0 ) continue;
                //削除条件してよい画素か確認
                if(isdeletePixel(x,y,verification,deleteFilter1,undeleteFilter1)){
                    //注目画素を白画素に変換
                    (renewal.ptr(y))[x] = 255; 
                    //1つ以上の画素が変更されたので終了しない
                    end_flag=false;
                }
            }
        }
        //上と右からの探査で変更箇所が存在しない場合には，処理を終了
        if(end_flag) break; 
        end_flag=true;
        //更新用画像を検証用画像にコピー
        verification=renewal.clone();
        //左と下から検索する場合
        for(int y=verification.rows-2;y>=1;y--){
            for(int x=1;x<verification.cols-1;x++){
                //注目画素が白の場合には，次の画素へと移動する
                if( (verification.ptr(y))[x] != 0 ) continue;
                //削除条件してよい画素か確認
                if(isdeletePixel(x,y,verification,deleteFilter2,undeleteFilter2)){
                    //注目画素を白画素に変換
                    (renewal.ptr(y))[x] = 255; 
                    //1つ以上の画素が変更されたので終了しない
                    end_flag=false;
                }
            }
        }
    }
    /*出力用のMatに細線化済みのMatを代入*/
    dst=verification.clone();
}
