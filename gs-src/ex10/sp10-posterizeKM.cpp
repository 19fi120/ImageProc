/*
Author              :Daisuke Kuwahara
Mail                :16fi501@ms.dendai.ac.jp
Created date        :2018/11/12
Version             :1.0
Contents(Japanese)  :ビット深度8の画像に対してk-means法を用いて，原定色表示を行うプログラム
How to use(Japanese):
<-- sp10-posterizeKM.exe A B -->
A   :使用するファイルの名前．                                   (例: exsample.jpg)
B   :分割数(クラスタの個数)．2個以上127個以下である必要がある． (例: 10)
*/

/*ヘッダ読み込み*/
#include<opencv2/opencv.hpp>
#include<iostream>
#include<random>
#include<cmath>

/*グローバルスコープに対する名前を定義*/
using namespace std;
using namespace cv;

/*データ型を作成*/
typedef struct default_bgr{                         //画素値をBGRに分割して格納する変数
    float b;            //Blue
    float g;            //Green
    float r;            //Red
} BGR;

typedef struct default_data{                        //データ点とクラスタを同時に格納する構造体
    BGR data;           //データ点
    int clusterType;    //クラスタID
} Data;

/*関数プロトタイプ*/

//ファイル名から画像をMat型で読み込む関数
inline Mat readImage(const string &readFileName);

//画像内に存在する画素の分布を計算し，引数に格納する関数
inline void initalize(const Mat src,vector<Data> &dst,const int clusterNum);

//クラスタ中心を計算する関数
inline void calcCentroid(const vector<Data> src,vector<BGR> &dst);

//データ点に対するクラスタの割り当てを，最も近い中心を持つクラスタへと移動する関数
inline bool changeCluster(vector<Data> &data,const vector<BGR> centroids);

//画素をクラスタ中の値に変更する関数
inline void changeColor(const Mat src,Mat &dst,const vector<Data> data,const vector<BGR> centroids);

/*関数実装*/

int main(int argc,char* argv[]){

    /*引数の例外処理*/
    if(argc<3){
        cerr << __func__ << ":引数は3個以上必要です．" << endl << "使用例:" << argv[0] << " ImageFileName.jpg 10" << endl;
        return -1;
    }
    int clusterNum=atoi(argv[2]);

    if(clusterNum<8){
        cerr << __func__ << ":クラスタの数は8個以上必要です．" << endl;
        return -2;
    }
    else if(clusterNum>2048383){
        cerr << __func__ << ":クラスタの数は2,048,383個以下です．" << endl;
        return -3;
    }
    
    /*画像の読み込み*/
    Mat img=readImage(string(argv[1]));

    /*各チャンネルのデータとクラスタを対応*/
    vector<Data> data;                              //データの値とクラスタを格納する変数
    initalize(img,data,clusterNum);

    /*k-means*/
    vector<BGR> clusterCentroid(clusterNum);        //クラスタ中心を格納するコンテナ

    cout << "クラスタリング開始" << endl;

    while(true){
        //クラスタの重心を計算
        calcCentroid(data,clusterCentroid);

        //クラスタ重心によって，各データのクラスタを更新
        if(!changeCluster(data,clusterCentroid)) break;
    }

    cout << "クラスタリング完了" << endl;

    /*画像をクラスタ中心に変更*/
    Mat output;
  
    changeColor(img,output,data,clusterCentroid);

    cout << "処理完了" << endl;

    /*画像の表示*/
    
    std::string result_win_title = "Posterized "+ to_string(clusterNum);

    imshow("Input",img);
    imshow(result_win_title,output);

    waitKey();

    return 0;
}

/*
ファイル名から画像をMat型で読み込む関数
引数    readFileName    :読み込む画像の名前
戻り値                  :Mat型の読み込んだ画像データ
*/
inline Mat readImage(const string &readFileName){
    Mat loadImage;

    /*引数に関する例外処理*/
    if(readFileName.empty()){
        //readFileNameに入力が存在しない
        cerr << __func__ << ":引数が空です．" << endl;
        return loadImage;
    }
    else if(readFileName.rfind(".")==-1){
        //拡張子が存在しない
        cerr << __func__ << ":引数に拡張子が存在しません．" << endl;
        return loadImage;
    }

    /*画像を読み込む*/
    loadImage=imread(readFileName);

    /*読み込んだ画像の例外処理*/
    if(loadImage.empty()){
        //画像が読み込めない
        cerr << __func__ << ":画像が読み込めません" << endl;
        return loadImage;
    }

    cout << readFileName << " から読み込み完了" << endl;

    return loadImage;
}

/*
データとクラスタの関係をランダムに初期化する関数
引数    src             :データ元の画像ファイル
引数    dst             :データとクラスタの対応が格納されるコンテナ
引数    clusterNum      :クラスタの数
*/
inline void initalize(const Mat src,vector<Data> &dst,const int clusterNum){
    cout << "初期化開始" << endl;
    
    /*引数に関する例外処理*/
    if(src.empty()){
        //入力画像が空
        cerr << __func__ << ":入力画像が不正です" << endl;
        return;
    }
    else if(clusterNum<6){
        //クラスタの数が不正
        cerr << __func__ << ":クラスタの数は2個以上必要です．" << endl;
        return;
    }
    else if(clusterNum>2048383){
        cerr << __func__ << ":クラスタの数は2,048,383個以下です．" << endl;
        return;    
    }

    /*画像から画素値を抽出*/
    bool alreadyFlag;       //すでに割り当て済みかどうかを示すフラグ
    random_device rnd;

    for(int y=0;y<src.rows;y++){
        const uchar *p=src.ptr(y);
        for(int x=0;x<src.cols;x++){
            alreadyFlag=true;

            //すでにクラスタが割り振られていないかを確認
            for(int i=0;i<dst.size();i++)
                if(p[x*3]==(uchar)dst[i].data.b && p[x*3+1]==(uchar)dst[i].data.g && p[x*3+2]==(uchar)dst[i].data.r)
                    alreadyFlag=false;    
            
            //割り振られていない場合
            if(alreadyFlag){
                //dstに追加
                dst.push_back({
                    {
                        (float)p[x*3],
                        (float)p[x*3+1],
                        (float)p[x*3+2],
                    },
                    -1
                });
            }
        }
    }

    //クラスタの数がデータ数を超えていないかを確認
    if(dst.size()<clusterNum){
        cerr << __func__ << ":分割数がデータ数を超えています．" << endl;
        return;
    }

    /*クラスタをランダムに振り分ける*/
    vector<BGR> centroids(clusterNum);

    //1つのデータに1つのクラスタを割り当てる
    for(int i=0;i<centroids.size();i++){
        int clusterCoordinates=rnd()%dst.size();    //初期のクラスタの中心座標となるdstの添え字
        
        centroids[i].b=dst[clusterCoordinates].data.b;
        centroids[i].g=dst[clusterCoordinates].data.g;
        centroids[i].r=dst[clusterCoordinates].data.r;
    }

    //割り当てたクラスタ中心から，データにクラスタを割り当て
    changeCluster(dst,centroids);

    cout << "初期化完了" << endl;
}

/*
クラスタ中心を計算する関数
引数    src             :計算ソースのData型が格納されたコンテナ
引数    dst             :クラスタ中心が格納されるコンテナ．必ずクラスタの個数のサイズである必要がある．
*/
inline void calcCentroid(const vector<Data> src,vector<BGR> &dst){
    /*変数の初期化*/
    vector<float> sum(dst.size(),0.0);
    for(int i=0;i<dst.size();i++){
        dst[i].b=0.0;
        dst[i].g=0.0;
        dst[i].r=0.0;
    }

    /*各クラスタのデータの総和を取得*/
    for(int i=0;i<src.size();i++){
        dst[src[i].clusterType].b+=src[i].data.b;
        dst[src[i].clusterType].g+=src[i].data.g;
        dst[src[i].clusterType].r+=src[i].data.r;

        sum[src[i].clusterType]+=1.0;
    }

    /*クラスタ中心(=重心=平均)を算出*/
    for(int i=0;i<dst.size();i++){
        dst[i].b/=sum[i];
        dst[i].g/=sum[i];
        dst[i].r/=sum[i];
    }
}

/*
データ点に対するクラスタの割り当てを，最も近い中心を持つクラスタへと移動する関数
引数    data            :変更対象のData型が格納されたコンテナ
引数    centroids       :クラスタ中心が格納されたコンテナ
戻り値                  :変更があった場合にはtrue,そうでない場合にはfalse
*/
inline bool changeCluster(vector<Data> &data,const vector<BGR> centroids){
    register float distance;        //距離を格納する変数
    register float min;             //最小の距離を保存する変数
    int minType;                    //最小の距離であるクラスタの番号を格納する変数
    bool changeFlag=false;          //データ点に対するクラスタの変更があったかを格納するフラグ

    for(int i=0;i<data.size();i++){
        /*変数の初期化*/
        min=
            sqrt(
                    ((centroids[0].b-data[i].data.b)*(centroids[0].b-data[i].data.b))+
                    ((centroids[0].g-data[i].data.g)*(centroids[0].g-data[i].data.g))+
                    ((centroids[0].r-data[i].data.r)*(centroids[0].r-data[i].data.r))
            );
        minType=0;

        /*データ点から最小の距離であるクラスタを検索*/
        for(int j=1;j<centroids.size();j++){
            //メモ：変数に(centroids[j].b-data[i].data.b)を代入して計算すると実行速度が下がります
            distance=
                sqrt(
                    ((centroids[j].b-data[i].data.b)*(centroids[j].b-data[i].data.b))+
                    ((centroids[j].g-data[i].data.g)*(centroids[j].g-data[i].data.g))+
                    ((centroids[j].r-data[i].data.r)*(centroids[j].r-data[i].data.r))
                );

            if(min>distance){
                min=distance;
                minType=j;
            }
        }

        /*クラスタの更新*/

        //新しいクラスタであるかを確認
        if(data[i].clusterType!=minType){
            //新しいクラスタを割り当て
            data[i].clusterType=minType;

            //変更したので，戻り値をtrueに変更
            changeFlag=true;
        }
    }
    return changeFlag;
}

/*
画素をクラスタ中の値に変更する関数
引数    src             :変更元の画像
引数    dst             :変更後の画像が格納される変数
引数    toneTable       :画素の変更表が格納されたコンテナ
*/
inline void changeColor(const Mat src,Mat &dst,const vector<Data> data,const vector<BGR> centroids){
    cout << "画素値を変換中" << endl;
    
    /*引数に関する例外処理*/
    if(src.empty()){
        //入力画像が空
        cerr << __func__ << ":入力画像が不正です" << endl;
        return;
    }
    else if(data.empty()){
        //クラスタの数が不正
        cerr << __func__ << ":画素とクラスタの対応変数が入力されていません．" << endl;
        return;
    }
    else if(centroids.empty()){
        //クラスタの数が不正
        cerr << __func__ << ":クラスタ中心の変数が入力されていません．" << endl;
        return;
    }
    
    /*画像データのコピー*/
    dst=src.clone();        //入力のコピーを出力とする
    
    /*画素値の変換*/
    for(int y=0;y<src.rows;y++){
        uchar *p=dst.ptr(y);
        for(int x=0;x<src.cols;x++){
            //線形探索で，対応する画素を検索
            for(Data d : data){
                if(p[x*3]==(uchar)d.data.b && p[x*3+1]==(uchar)d.data.g && p[x*3+2]==(uchar)d.data.r){
                    //画素値をクラスタ中心に変換
                    p[x*3]=(uchar)centroids[d.clusterType].b;
                    p[x*3+1]=(uchar)centroids[d.clusterType].g;
                    p[x*3+2]=(uchar)centroids[d.clusterType].r;

                    break;
                }
            }
        }
    }

    cout << "画素値を変換完了" << endl;
}