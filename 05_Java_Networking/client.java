// mhamade - Mustapha Hamade

import java.io.*;
import java.net.*;
import java.util.*;
import static java.lang.System.*;
class client {
  static List<String> messageList=new ArrayList<String>();
  static Scanner scan = new Scanner(System.in);
  static class receiver implements Runnable {
    Socket sock;
    Scanner scan;
    receiver(Socket socket, Scanner scanner){
      sock = socket;
      scan = scanner;
    }
    public void run () {
      while (! sock.isInputShutdown() && scan.hasNextLine() ){
        //scans from socket
        String message = scan.nextLine();
        System.out.println(message);
      }
      scan.close();
    }
  }
  
   static class sender implements Runnable {
      chatter.options opts;
      PrintWriter writer;
      Scanner scanner;
      sender (chatter.options opts, PrintWriter writer) {
         this.opts = opts;
         this.writer = writer;
      }

      public void run () {
         scanner = new Scanner (System.in);
         writer.printf ("%s: %n", opts.username);
         writer.flush ();
         while (scanner.hasNextLine ()) {
            writer.printf ("%s%n", scanner.nextLine ());
            writer.flush ();
         }
         scanner.close ();
         writer.close ();
      }
   }
 
  
   static class listener implements Runnable {
      chatter.options opts;
      Scanner scanner;
      listener (chatter.options opts, Scanner scanner) {
         this.opts = opts;
         this.scanner = scanner;
      }

      public void run () {
         while (scanner.hasNextLine ()) {
            out.printf ("%s%n", scanner.nextLine ());
         }   

         scanner.close ();
      }
   }

     static class client_start implements Runnable {
      chatter.options opts;
      client_start (chatter.options opts) {
         this.opts = opts;
      }

      public void run () {
         try {
            Socket socket = new Socket (opts.server_hostname,
                                        opts.server_portnumber);
            out.printf ("Socket OK%n");
            Thread sender = new Thread (new sender (opts, 
               new PrintWriter (socket.getOutputStream ())));
            Thread listener = new Thread (new listener (opts,
               new Scanner (socket.getInputStream ())));
            sender.start ();
            listener.start ();
         }catch (IOException e) {
            System.out.println("Client warning: " + e);
            System.exit(1);
         }
      }
   }
}
