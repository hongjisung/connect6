import java.awt.*;
import java.awt.event.*;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import javax.swing.*;
public class Main extends JFrame implements Runnable, KeyListener {

    //private int x = 30, y = 30;
    private int o_xpos = 0, o_ypos = 0;
    private int keyStatus = 0;
    private int old_keyStatus = 0;
    private int chknum = 0;
    private int omok_turn = 0 ;

    private boolean twoStone = false;
    private int twoStoneOrder = 0;

    private int omok_size = 19;
    private int uWid = 30;
    private int keycnt = 0;
    private int key_proc = 0;
    private int[][] omok_arr = new int[19][19] ;

    public Main() {

        info = new int[2][361][20][20];

        // JFrame jFrame = new JFrame();
        draw drawing = new draw();
        this.add(drawing);
        this.addKeyListener(this);

        //this.setSize(600, 630);
        if( omok_size >= 19 ) omok_size = 19 ;
        else if( omok_size <= 13 ) omok_size = 13 ;
        o_xpos = omok_size/2 ;
        o_ypos = omok_size/2 ;


        this.setSize((omok_size+1)*uWid + uWid/6, (omok_size+1)*uWid + uWid);


        for( int i= 0 ; i<19 ; i++ ) for( int j= 0 ; j<19 ; j++ ) omok_arr[i][j] = 0 ;

        this.setTitle("Omok");

        this.setResizable(false);
        this.setBackground(new Color(164,128,0));
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setVisible(true);
    }

    public void run() {
        try {
            while(true) {

                if( keyStatus != 0 )
                {
                    if( keyStatus != old_keyStatus )
                    {
                        keycnt = -40 ;
                        key_proc = 1 ;
                    }
                    else if( keycnt >= 10 )
                    {
                        keycnt = 0 ;
                        key_proc = 1 ;
                    }
                    else
                    {
                        key_proc = 0 ;
                    }

                    if( key_proc == 1 )
                    {
                        chknum = 0 ;
                        do
                        {
                            if((keyStatus & 0x0001) == 0x0001 ) {
                                if ( o_xpos < (omok_size-1) ) ++o_xpos ;
                                else o_xpos = 0 ;
                            }
                            if((keyStatus & 0x0010) == 0x0010 ){
                                if ( o_xpos > 0 ) --o_xpos ;
                                else o_xpos = omok_size - 1 ;
                            }
                            if((keyStatus & 0x1000) == 0x1000 ){
                                if ( o_ypos < (omok_size-1) ) ++o_ypos ;
                                else o_ypos = 0 ;
                            }
                            if((keyStatus & 0x0100) == 0x0100 ){
                                if ( o_ypos > 0 ) --o_ypos ;
                                else o_ypos = omok_size - 1 ;
                            }
                            ++chknum ;
                        }
                        while( ( omok_arr[o_ypos][o_xpos] != 0) && (chknum < omok_size)) ;

                        if((keyStatus & 0x0002) == 0x0002 ){
                            if( omok_arr[o_ypos][o_xpos] == 0) {
                                if( omok_turn == 0 ) {

                                    if(twoStone){
                                        if(twoStoneOrder == 0){
                                            omok_turn = 0;
                                            twoStoneOrder++;
                                        }else{
                                            twoStoneOrder = 0;
                                            omok_turn = 1;
                                        }
                                    }else{
                                        omok_turn = 1;
                                    }

                                    omok_arr[o_ypos][o_xpos] = 1 ;
                                    updateInfo();
                                    check(1,o_ypos,o_xpos);
                                }
                                else {

                                    if(twoStone){
                                        if(twoStoneOrder == 0){
                                            omok_turn = 1;
                                            twoStoneOrder++;
                                        }else{
                                            twoStoneOrder = 0;
                                            omok_turn = 0;
                                        }
                                    }else{
                                        omok_turn = 0;
                                    }

                                    omok_arr[o_ypos][o_xpos] = 2 ;
                                    updateInfo();
                                    check(2,o_ypos,o_xpos);
                                }

                                turnNum++;
                                if(turnNum == 2){
                                    twoStone = true;
                                }

                            }
                        }

                        repaint();
                        //Thread.sleep(10);
                    }
                    //System.out.println(keycnt) ;
                }
                old_keyStatus = keyStatus ;
                ++keycnt ;

                Thread.sleep(10);
            }
        }catch (Exception e){}
    }

    public void keyPressed(KeyEvent e){
        if(e.getKeyCode()==e.VK_RIGHT) keyStatus |= 0x0001;
        if(e.getKeyCode()==e.VK_LEFT)  keyStatus |= 0x0010;
        if(e.getKeyCode()==e.VK_UP)    keyStatus |= 0x0100;
        if(e.getKeyCode()==e.VK_DOWN)  keyStatus |= 0x1000;
        if(e.getKeyCode()==e.VK_ENTER)  keyStatus |= 0x0002;
    }
    public void keyReleased(KeyEvent e)
    {
        if(e.getKeyCode()==e.VK_RIGHT) keyStatus -= 0x0001;
        if(e.getKeyCode()==e.VK_LEFT)  keyStatus -= 0x0010;
        if(e.getKeyCode()==e.VK_UP)    keyStatus -= 0x0100;
        if(e.getKeyCode()==e.VK_DOWN)  keyStatus -= 0x1000;
        if(e.getKeyCode()==e.VK_ENTER)  keyStatus -= 0x0002;
    }
    public void keyTyped(KeyEvent ke) {}


    public class draw extends JPanel {
        public void paintComponent(Graphics g){
            g.setColor(Color.RED);
            g.fillOval((o_xpos+1)*uWid - uWid/6 , (o_ypos+1)*uWid-uWid/6, uWid/3, uWid/3);

            g.setColor(new Color(0,0,0));
            for( int i = 0 ; i<omok_size ; i++ )
            {
                g.drawLine(uWid, (i+1)*uWid, omok_size * uWid, (i+1)*uWid);
                g.drawLine((i+1)*uWid, uWid, (i+1)*uWid, omok_size * uWid);
            }
            for( int i = 0 ; i<omok_size ; i++ )
            {
                for( int j = 0 ; j<omok_size ; j++ )
                {
                    if( omok_arr[i][j] != 0 )
                    {
                        if( omok_arr[i][j] == 1 )	g.setColor(new Color(0,0,0)); 		// black
                        else 						g.setColor(new Color(255,255,255)); // white

                        g.fillOval(uWid + j*uWid - 3*uWid/7, uWid + i*uWid -3*uWid/7, 6*uWid/7, 6*uWid/7);
                    }
                }
            }
        }
    }

    public void check(int who, int yPos, int xPos){

        int opposite;
        if(who == 1){
            opposite = 2;
        }else{
            opposite = 1;
        }

        int y,x;
        //up-down
        int cnt = 1;
        //up
        y = yPos-1;
        while(y>= 0 && omok_arr[y][xPos] == who){
            cnt++;
            y--;
        }
        //down
        y = yPos+1;
        while(y<=omok_size-1 && omok_arr[y][xPos] == who){
            cnt++;
            y++;
        }
        if(cnt == 7){ // 7목 실격패!!
            System.out.println(who+" 7목 실격 패");
            saveResult(opposite);
        }else if(cnt == 6){ //6목 승리!!
            System.out.println(who+" 6목 승릭");
            saveResult(who);
        }

        //left-right
        cnt = 1;
        y = yPos;
        //left
        x = xPos-1;
        while(x>=0 && omok_arr[yPos][x] == who){
            cnt++;
            x--;
        }
        //right
        x = xPos+1;
        while(x<=omok_size-1 && omok_arr[yPos][x] == who){
            cnt++;
            x++;
        }
        if(cnt == 7){ // 7목 실격패!!
            System.out.println(who+" 7목 실격 패");
            saveResult(opposite);
        }else if(cnt == 6){ //6목 승리!!
            System.out.println(who+" 6목 승릭");
            saveResult(who);
        }

        //left-Up -> right-Down
        cnt = 1;
        y = yPos-1;
        x = xPos-1;
        while(y>=0 && x>=0 && omok_arr[y][x] == who){
            cnt++;
            y--;
            x--;
        }
        y = yPos+1;
        x = xPos+1;
        while(y<=omok_size-1 && x<=omok_size-1 && omok_arr[y][x] == who){
            cnt++;
            y++;
            x++;
        }
        if(cnt == 7){ // 7목 실격패!!
            System.out.println(who+" 7목 실격 패");
            saveResult(opposite);
        }else if(cnt == 6){ //6목 승리!!
            System.out.println(who+" 6목 승릭");
            saveResult(who);
        }

        //left-Down -> right-Up
        cnt = 1;
        y = yPos+1;
        x = xPos-1;
        while(y<=omok_size-1 && x>=0 && omok_arr[y][x] == who){
            cnt++;
            y++;
            x--;
        }
        y = yPos-1;
        x = xPos+1;
        while(y>=0 && x<=omok_size-1 && omok_arr[y][x] == who){
            cnt++;
            y--;
            x++;
        }
        if(cnt == 7){ // 7목 실격패!!
            System.out.println(who+" 7목 실격 패");
            saveResult(opposite);
        }else if(cnt == 6){ //6목 승리!!
            System.out.println(who+" 6목 승릭");
            saveResult(who);
        }
    }

    public void saveResult(int winner){

        File file = new File("./test.txt");
        try{
            BufferedWriter bw = new BufferedWriter(new FileWriter(file));

            for(int i=6; i<=19; i++){
                for(int j=1; j<=i; j++){
                    bw.write('0');
                    if(i<10){
                        bw.write('0');
                        bw.write(String.valueOf(i));
                    }else{
                        bw.write(String.valueOf(i));
                    }
                    if(j<10){
                        bw.write('0');
                        bw.write(String.valueOf(j));
                    }else{
                        bw.write(String.valueOf(j));
                    }
                    bw.write(",");
                }
            }

            for(int i=6; i<=19; i++){
                for(int j=1; j<=i; j++){
                    bw.write('1');
                    if(i<10){
                        bw.write('0');
                        bw.write(String.valueOf(i));
                    }else{
                        bw.write(String.valueOf(i));
                    }
                    bw.write(String.valueOf(j));
                    bw.write(",");
                }
            }
            bw.write("RESULT\n");

            if(winner == 1){
                for(int i=0; i<turnNum; i++){
                    for(int j=6; j<=19; j++){
                        for(int r=1; r<=j; r++){
                            int num = info[0][i][j][r];
                            bw.write(num+",");
                        }
                    }
                    for(int j=6; j<=19; j++){
                        for(int r=1; r<=j; r++){
                            int num = info[1][i][j][r];
                            bw.write(num+",");
                        }
                    }
                    bw.write("WIN\n");
                    bw.flush();
                }

                for(int i=0; i<turnNum; i++){
                    for(int j=6; j<=19; j++){
                        for(int r=1; r<=j; r++){
                            int num = info[1][i][j][r];
                            bw.write(num+",");
                        }
                    }
                    for(int j=6; j<=19; j++){
                        for(int r=1; r<=j; r++){
                            int num = info[0][i][j][r];
                            bw.write(num+",");
                        }
                    }
                    bw.write("LOSE\n");
                    bw.flush();
                }
            }else if(winner == 2){
                for(int i=0; i<turnNum; i++){
                    for(int j=6; j<=19; j++){
                        for(int r=1; r<=j; r++){
                            int num = info[1][i][j][r];
                            bw.write(num+",");
                        }
                    }
                    for(int j=6; j<=19; j++){
                        for(int r=1; r<=j; r++){
                            int num = info[0][i][j][r];
                            bw.write(num+",");
                        }
                    }
                    bw.write("WIN\n");
                    bw.flush();
                }

                for(int i=0; i<turnNum; i++){
                    for(int j=6; j<=19; j++){
                        for(int r=1; r<=j; r++){
                            int num = info[0][i][j][r];
                            bw.write(num+",");
                        }
                    }
                    for(int j=6; j<=19; j++){
                        for(int r=1; r<=j; r++){
                            int num = info[1][i][j][r];
                            bw.write(num+",");
                        }
                    }
                    bw.write("LOSE\n");
                    bw.flush();
                }
                bw.close();
            }

        }catch (Exception e){
            e.printStackTrace();
        }
        System.exit(0);

    }

    private int[][][][] info; // 2*361*20*20 각 턴에서 who 마다 i칸(빈칸+who칸)중 who돌 j개   who 1->0 2->1
    private int turnNum = 0;

    public void updateInfo(){
        int who;
        //who 가 1로 바라볼때  2는 벽
        who = 1;
        //가로줄 -> 19줄

        for(int y=0; y<19; y++){
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone

            for(int x=0; x<19; x++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }

        //세로줄 -> 19줄
        for(int x=0; x<19; x++){
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int y=0; y<19; y++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }

        //왼위오아 대각선줄
        for(int t=0; t<19; t++){
            int x = t;
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int y=0; y<19-t; y++, x++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }

        for(int t=1; t<19; t++){
            int y = t;
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int x=0; x<19-t; x++, y++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }
        //왼아오위 대각선줄
        for(int t=0; t<19; t++){
            int y = t;
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int x=0; x<t+1; x++, y--){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }

        for(int t=1; t<19; t++){
            int x = t;
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int y=19-t; y>=t; y--,x++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }

        //who 가 2로 바라볼때

        who = 2;
        //가로줄 -> 19줄
        for(int y=0; y<19; y++){
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int x=0; x<19; x++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }
        //세로줄 -> 19줄
        for(int x=0; x<19; x++){
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int y=0; y<19; y++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }
        //왼위오아 대각선줄
        for(int t=0; t<19; t++){
            int x = t;
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int y=0; y<19-t; y++, x++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }


        }

        for(int t=1; t<19; t++){
            int y = t;
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int x=0; x<19-t; x++, y++){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }

        //왼아오위 대각선줄
        for(int t=0; t<19; t++){
            int y = t;
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for(int x=0; x<t+1; x++, y--){
                if(state == 0){
                    if(omok_arr[y][x] == who){
                        state = 1;
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        state = 1;
                        empty++;
                    }
                }else if(state == 1){
                    if(omok_arr[y][x] == who){
                        myStone++;
                    }else if(omok_arr[y][x] == 0){
                        empty++;
                    }else{
                        state = 0;
                        info[who-1][turnNum][empty+myStone][myStone]++;
                    }
                }
            }
            if(state == 1){
                info[who-1][turnNum][empty+myStone][myStone]++;
            }
        }

        for(int t=1; t<19; t++) {
            int x = t;
            int empty = 0;
            int myStone = 0;
            int state = 0; //0->wall 1->empty+mystone
            for (int y = 19 - t; y >= t; y--, x++) {
                if (state == 0) {
                    if (omok_arr[y][x] == who) {
                        state = 1;
                        myStone++;
                    } else if (omok_arr[y][x] == 0) {
                        state = 1;
                        empty++;
                    }
                } else if (state == 1) {
                    if (omok_arr[y][x] == who) {
                        myStone++;
                    } else if (omok_arr[y][x] == 0) {
                        empty++;
                    } else {
                        state = 0;
                        info[who - 1][turnNum][empty + myStone][myStone]++;
                    }
                }
            }
            if (state == 1) {
                info[who - 1][turnNum][empty + myStone][myStone]++;
            }
        }

        //System.out.println(info[0][turnNum][19][1]);
    }

    public static void main(String[] args) {
        Thread t = new Thread(new Main());
        t.start();
    }
}

