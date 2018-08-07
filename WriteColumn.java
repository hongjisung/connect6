import java.io.*;
import java.util.StringTokenizer;

public class WriteColumn {
    public static void main(String[] args) throws Exception {
        File file = new File("./data.csv");
        try {
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
                    if(j<10){
                        bw.write('0');
                        bw.write(String.valueOf(j));
                    }else{
                        bw.write(String.valueOf(j));
                    }
                    bw.write(",");
                }
            }
            bw.write("RESULT\n");
            bw.flush();
            bw.close();

        } catch (Exception e) {

        }
    }
}
