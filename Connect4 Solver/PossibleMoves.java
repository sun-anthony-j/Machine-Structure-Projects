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

public class PossibleMoves {
    public static class Map extends Mapper<IntWritable, MovesWritable, IntWritable, IntWritable> {
        int boardWidth;
        int boardHeight;
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
            OTurn = context.getConfiguration().getBoolean("OTurn", true);
        }

        /**
         * The map function for the first mapreduce that you should be filling out.
         */
        @Override
        public void map(IntWritable key, MovesWritable val, Context context) throws IOException, InterruptedException {
            /* YOU CODE HERE */
            String temp = Proj2Util.gameUnhasher(key.get(), boardWidth, boardHeight);
            //char chip = (OTurn)?'O':'X';
            char[] parent = temp.toCharArray();
            // System.out.println("Status:"+val.getStatus());
            if(val.getStatus() != 0) {
                return;
            }
      //   for (int i = 0; i < boardWidth * boardHeight; i++) {
            // chip = (OTurn)?'O':'X';
            // if (i % boardHeight == 0) {// check if the bottom filled out, there are n bottoms where n = boardWidth we only want to keep track of each colum
            //     if (parent[i] == ' ') { // if not return 'X' or 'O'
            //      parent[i] = chip;
            //      String child = new String(parent);
            //      parent[i] = ' ';
            //      IntWritable childHash = new IntWritable();
            //      childHash.set(Proj2Util.gameHasher(child,boardWidth,boardHeight));// hash back into int
            //      context.write(childHash, key);
            //     } 
            // } else { // if the bottom is filled out then check the next top to see if it's filled out
            //  for (int j = i + 1; j <boardHeight; j++) {// check the entire column
            //      if (parent[j] == ' ') {// if there is a empty space
            //          parent[j] = chip; // put 'X' or 'O' into it
            //          String child = new String(parent); // convert array of char to string child
            //          parent[j] = ' ';
            //          IntWritable childHash = new IntWritable();
            //          childHash.set(Proj2Util.gameHasher(child,boardWidth,boardHeight));// hash back into int
            //          System.out.println("key: ("+ temp +") Map, child: ("+ child+")");
            //          context.write(childHash, key);
            //          break; // done with the check
            //      } else {
      //                continue; // continue to check the entire column
            //      }
            //  }
            // }
      //   }

            for (int i =0; i<boardWidth; i++){ //for each column
                // System.out.println("i="+ i);
                breakpoint:
                for (int j = 0; j <boardHeight; j++){
                    if (parent[i*boardHeight + j]==' '){
                        
                        if (OTurn){
                            parent[i*boardHeight + j] = 'O';
                        }else{
                            parent[i*boardHeight + j] = 'X';
                        }
                        String child = new String (parent);
                        // if ((child == new String("XXOOXO   ")) ||(temp ==new String("XXOOXO   "))){
                        //     String temp2 = Proj2Util.gameUnhasher(key2.get(), boardWidth, boardHeight);
                        //     System.out.println("Child:"+child+"temp:"+temp2);
                        // }
                        // System.out.println("added:"+parent[i*boardHeight + j]);
                        parent[i*boardHeight + j] = ' ';
                        IntWritable childHash = new IntWritable();
                        childHash.set(Proj2Util.gameHasher(child,boardWidth,boardHeight));// hash back into int
                        // System.out.println("key: ("+ temp +") Map, child: ("+ child+")");
                        context.write(childHash, key);
                        break breakpoint;
                    }
                }
            }
        }
    }
    public static class Reduce extends Reducer<IntWritable, IntWritable, IntWritable, MovesWritable> {

    int boardWidth;
    int boardHeight;
    int connectWin;
    boolean OTurn;
    boolean lastRound;
    /**
     * Configuration and setup that occurs before reduce gets called for the first time.
     *
     **/
    @Override
        public void setup(Context context) {
        // load up the config vars specified in Proj2.java#main()
        boardWidth = context.getConfiguration().getInt("boardWidth", 0);
        boardHeight = context.getConfiguration().getInt("boardHeight", 0);
        connectWin = context.getConfiguration().getInt("connectWin", 0);
        OTurn = context.getConfiguration().getBoolean("OTurn", true);
        lastRound = context.getConfiguration().getBoolean("lastRound", true);
    }

    /**
     * The reduce function for the first mapreduce that you should be filling out.
     */
    @Override
        public void reduce(IntWritable key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        /* YOU CODE HERE */
        MovesWritable gamestate = new MovesWritable();
        String child = Proj2Util.gameUnhasher(key.get(), boardWidth, boardHeight);
        // System.out.println("Board:"+child);
        // int parentLength = 0;
        int count = 0;
        // Iterator<IntWritable> itr = values.iterator();
        // while (itr.hasNext()) {
        //     parentLength++;
        //     IntWritable asdf = itr.next();
        //     System.out.println("PossibleReduce Parent Board:"+Proj2Util.gameUnhasher(asdf.get(), boardWidth, boardHeight));
        //     System.out.println("NUMBER OF PARENTS:"+parentLength);
        // }
        int[] parents = new int[boardWidth];

        for (IntWritable value :values) {
        //map all parents into the array[] move
        // if (count < parentLength) {
            parents[count] = value.get();
            // System.out.println("PossibleReduce Parent Board:"+Proj2Util.gameUnhasher(value.get(), boardWidth, boardHeight));
            count++;
        // }
        }
        int[] parents2 = new int[count];
        for (int i= 0; i<count; i++){
            parents2[i]=parents[i];
        }

        gamestate.setMoves(parents2);// this takes array..
        gamestate.setMovesToEnd(0); // set the first 6 bits to zero
        //char chip = (OTurn)?'O':'X';

        if ((lastRound) && (!Proj2Util.gameFinished(child, boardWidth, boardHeight, connectWin))) {
        // if tie set to 3;
            gamestate.setStatus(3);
        //System.out.println("PING");
        }
        if ((!lastRound) && (!Proj2Util.gameFinished(child, boardWidth, boardHeight, connectWin))) {
        // if undecide set to 0
            gamestate.setStatus(0);
        //System.out.println("PING");
        }
        if ((OTurn) && (Proj2Util.gameFinished(child, boardWidth, boardHeight, connectWin))) {
        // if O wins set to 1
            gamestate.setStatus(1);
        
        }
        if ((!OTurn) && (Proj2Util.gameFinished(child, boardWidth, boardHeight, connectWin))) {
        // if X wins set to 2
            gamestate.setStatus(2);

        }
        context.write(key, gamestate);
    }
    }
}   

               
