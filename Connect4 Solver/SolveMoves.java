/*
 * CS61C Spring 2014 Project2
 * Reminders:
 *
 * DO NOT SHARE CODE IN ANY WAY SHAPE OR FORM, NEITHER IN PUBLIC REPOS OR FOR DEBUGGING.
 *
 * This is one of the two files that you should be modifying and submitting for this project.
 */
import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.lang.Math;

import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;

public class SolveMoves {
    public static class Map extends Mapper<IntWritable, MovesWritable, IntWritable, ByteWritable> {
        /**
         * Configuration and setup that occurs before map gets called for the first time.
         *
         **/
        @Override
        public void setup(Context context) {
            
        }

        /**
         * The map function for the second mapreduce that you should be filling out.
         */
        @Override
        public void map(IntWritable key, MovesWritable val, Context context) throws IOException, InterruptedException {
            /* YOUR CODE HERE */
            int boardWidth = context.getConfiguration().getInt("boardWidth", 0);
            int boardHeight = context.getConfiguration().getInt("boardHeight", 0);
            String temp = Proj2Util.gameUnhasher(key.get(), boardWidth, boardHeight);
            // System.out.println("Mapkey:"+key.get());
            int[] parents = val.getMoves();
            byte value = val.getValue();
            ByteWritable value2 = new ByteWritable(value);

            // Need to filter for bad parents generated by solvemovesReduce
            // Nevermind, checking in reduce
            //if (val.getMovestoEnd() == 0){ //If val was generated from possiblemoves
                for (int i=0; i<parents.length; i++){
                    IntWritable key2 = new IntWritable(parents[i]);
                    // System.out.println("Mapoutput:"+key2.get());
                    // if (temp == new String("XXOOXO   ")){
                        // String temp2 = Proj2Util.gameUnhasher(key2.get(), boardWidth, boardHeight);
                        // System.out.println("Child:"+temp+"Parent:"+temp2);
                    // }
                    context.write(key2, value2);
                }
            //}else{
            
            }
    }

    public static class Reduce extends Reducer<IntWritable, ByteWritable, IntWritable, MovesWritable> {

        int boardWidth;
        int boardHeight;
        int connectWin;
        boolean OTurn;
        /**
         * Configuration and setup that occurs before map gets called for the first time.
         *
         **/
        @Override
        public void setup(Context context) {
            // load up the config vars specified in Proj2.java#main()
            boardWidth = context.getConfiguration().getInt("boardWidth", 0);
            boardHeight = context.getConfiguration().getInt("boardHeight", 0);
            connectWin = context.getConfiguration().getInt("connectWin", 0);
            OTurn = context.getConfiguration().getBoolean("OTurn", true);
        }

        /**
         * The reduce function for the first mapreduce that you should be filling out.
         */
        @Override
        public void reduce(IntWritable key, Iterable<ByteWritable> values, Context context) throws IOException, InterruptedException {
            /* YOUR CODE HERE */
            String output = Proj2Util.gameUnhasher(key.get(), boardWidth, boardHeight);
            // System.out.println("Board:"+ output);

            int status = 0;
            int moves = -1;
            boolean valid = false;
            //// Filter out impossible states
            //for (ByteWritable value:values) {
            //  if (outcome == 0){
            //      valid = true;
            //  }
            //}
            //  if (!valid){
            //      return;
            //  }
            
            // int undecided = 0;
            // int total = 0;
            // int[] validParents= new int[boardWidth];
            // int count =0;


            
            for (ByteWritable value:values) {

                int outcome = value.get() & 3;
                int moves2 = value.get() >> 2;
                // if (output == new String("XXOOXO   ")){
                //     System.out.println("Outcome:"+outcome+"moves:"+moves2);
                // }
                // System.out.println("Outcome:"+outcome+"moves:"+moves2);
                // total++;
                if (moves2 == 0){ //if we get a node from possiblemoves?
                    valid =true;
                }
                // if (outcome ==0){
                //     return;
                // }
                if (OTurn){
                    if (outcome == 1){ //Owin
                        if ((moves == -1) || (moves > moves2) || status !=1){
                            moves = moves2;
                        }
                        status = 1;
                    }
                    else if ((outcome == 3) && (status != 1)){ //Tie
                        
                        if ((moves == -1) || (moves < moves2) || status !=3){
                            moves = moves2;
                        }
                        status = 3;
                    }
                    else if ((outcome == 2) && (status != 1) && (status !=3)){ //OLoss
                        status = 2;
                        if ((moves == -1) || (moves < moves2)){
                            moves = moves2;
                        }
                    }
                }else{ // if X's turn
                    if (outcome == 2){ //Xwin
                        
                        if ((moves == -1) || (moves > moves2)|| status !=2){
                            moves = moves2;
                        }
                        status = 2;
                    }
                    else if ((outcome == 3)&&(status !=2)){ //Tie
                        if ((moves == -1) || (moves < moves2) || status !=3){
                            moves = moves2;
                        }
                        status = 3;
                    }
                    else if ((outcome == 1)&& (status != 3) && (status !=2)){ //Xloss
                        
                        if ((moves == -1) || (moves < moves2)||status !=1){
                            moves = moves2;
                        }
                        status = 1;
                    }
                }
            }
            // System.out.println("undecided:"+undecided);
            if (!valid){
                return;
            }
            // if (undecided != 0){
            //     return;
            // }

            if (moves == -1){ //something not right, come back later
                return;
            }
            // Make output moveswritable
            MovesWritable gamestate = new MovesWritable();
            gamestate.setStatus(status);
            gamestate.setMovesToEnd(moves+1);
            // make parents 
            String temp0 = Proj2Util.gameUnhasher(key.get(), boardWidth, boardHeight);
            char[] temp = temp0.toCharArray();
            String[] temp2 = new String[boardWidth];
            char[] temp3;
            int parentcount = 0;
            // System.out.println("Board:" + temp0);
            for (int i =0; i<boardWidth; i++){ //for each column
                for (int j = boardHeight -1; j >= 0; j--){
                     // System.out.print("char at index"+(i*boardHeight + j)+":"+temp[i*boardHeight + j]);
                    //Try to remove the top piece
                    if (OTurn){
                        if (temp[i*boardHeight + j]=='X'){
                            temp3 = java.util.Arrays.copyOf(temp, boardWidth*boardHeight);
                            temp3[i*boardHeight + j] = ' ';
                            temp2[parentcount] = new String (temp3);
                            // System.out.println("OTURN:"+temp2[parentcount]);
                            parentcount++;
                            break;
                        }
                    }else{
                        if (temp[i*boardHeight + j]=='O'){
                            temp3 = java.util.Arrays.copyOf(temp, boardWidth*boardHeight);
                            temp3[i*boardHeight + j] = ' ';
                            temp2[parentcount] = new String (temp3);
                            // System.out.println(OTurn+"XTURN:"+temp2[parentcount]);
                            parentcount++;
                            break;
                        }
                    }
                    
                }
            }
            if (parentcount == 0){
                return;
            }

            int[] parents = new int[parentcount];
            for (int i = 0; i<parentcount; i++){
                // System.out.println("Child Board:("+key+ ")Parent Board:("+temp2[i]+")");
                parents[i]=Proj2Util.gameHasher(temp2[i], boardWidth, boardHeight);
            }
            gamestate.setMoves(parents);
            context.write(key, gamestate);
            
        }
    }
}
