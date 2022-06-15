void frameSub(int cameraNumber){
    int coutn = 0; // フレーム番号のカウント
    : : : :
    while (true) {
        cap >> frame; // カメラからの画像を取得
        : : : :
        if (count%n == 0){
            : : : : // フレーム間差分の処理
            frame2 = frame1.clone(); // 次回に備え移動
            frame1 = frame.clone(); // 次回に備え移動
}
count++;
: : : : // d,n の増減などのキー入力受付
}


int main(int argc, char* argv[]) {
    if (argc < 2) backSub(0); //引数省略時のdefault設定
    else backSub(atoi(argv[1])); // カメラ番号
return 0;
}