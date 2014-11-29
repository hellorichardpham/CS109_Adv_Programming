// mhamade - Mustapha Hamade

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import static java.lang.System.*;

class server {
  static List<String> messageList=new ArrayList<String>();
  private static ServerSocket socket;
  static userManager usertable;
  static BlockingQueue <String> buffer;

  server (chatter.options opts) {
  }
  
  static class server_listen implements Runnable {
      private ServerSocket socket;
      private Socket client;
      chatter.options opts;
      server_listen (chatter.options opts) {
         this.opts = opts;
      }

      public void run () {
         usertable = new userManager ();
         buffer =  new LinkedBlockingQueue <String> ();

         try {
            socket = new ServerSocket (opts.server_portnumber);
            Thread qm = new Thread (new queueManager (buffer));
            qm.setDaemon (true);
            qm.start ();
            out.printf ("Client connecting...%n");
            for (;;) {
               client = socket.accept ();
               out.printf ("Socket is accepted!%n");
               Thread vu = new Thread (new verify_user (opts, client));
               vu.start ();
            }
         }catch (IOException e) {
            System.out.println("Server warning: " + e);
            System.exit(1);
        }
      }
   }

   static class reader implements Runnable {
      Socket socket;
      Scanner scanner;
      reader (Socket _socket, Scanner _scanner) {
         scanner = _scanner;
         socket = _socket;
      }
      public void run () {
         while (! socket.isInputShutdown() && scanner.hasNextLine ()) {
            out.printf ("%s%n", scanner.nextLine ());
         }
         scanner.close ();
      }
   }
   
    static void put (BlockingQueue <String> source, String msg) {
        try {
            source.put (msg);
        }catch (InterruptedException e) {
            System.out.println("Server reader warning: " + e);
            System.exit(1);
      }
   }
   
   static class userManager {
        private static Map <String, Boolean> online_user;
        private static Map <String, PrintWriter> user_client_writer;
        userManager () {
            online_user = Collections.synchronizedMap 
                (new HashMap <String, Boolean> ());
            user_client_writer = Collections.synchronizedMap 
                (new HashMap <String, PrintWriter> ());
        }

    static boolean exists_user (String user) {
        boolean bool;
        bool = online_user.containsKey (user);
        return bool;
    }

      static boolean status_user (String user) {
         boolean bool;
         bool = online_user.get (user);
         return bool;
      }

      static void set_false_user (String user) {
         online_user.put (user, false);
      }

      static void add_user (String user) {
         online_user.put (user, true);
      }
     
      static void rm_user (String user) {
         boolean bool;  
         bool = online_user.remove (user);
      } 

      static void add_clientWriter (String user, PrintWriter pw) {
         user_client_writer.put (user, pw);
      }

      static PrintWriter rm_clientWriter (String user) {
         PrintWriter pw;
         pw = user_client_writer.remove (user);
         return pw;
      }    
    

      static Collection <PrintWriter> get_vals_cw () {
         Collection <PrintWriter> vals = user_client_writer.values ();
         return vals;
      }
   }
   
   static String poll (BlockingQueue <String> source, long msec) {
      String msg = null;
      
      try {
         msg = source.poll (msec, TimeUnit.MILLISECONDS);
      }catch (InterruptedException exn) {
         msg = null;  
      }
      return msg;
   }
   
   static class queueManager implements Runnable {
      BlockingQueue <String> source;
      queueManager (BlockingQueue <String> source) {
         this.source = source;
      }

      public void run () {
         for (;;) {
            String line = poll (source, 300);
            if (line == null) continue;
            for (PrintWriter client_write : usertable.get_vals_cw ()) {
               client_write.printf("%s%n", line);
               client_write.flush ();
            }
         }
      }
   }
   
   static class verify_user implements Runnable {
      chatter.options opts;
      private Socket client;
      verify_user (chatter.options opts, Socket client) {
         this.opts = opts;
         this.client = client;
      }

      public void run () {
         try {
            Scanner client_in = new Scanner (client.getInputStream ());
            String[] new_user = client_in.nextLine ().split (":", 2);

            if (usertable.exists_user (new_user[0]) == true) {
               throw new IOException ();
            }

            opts.username = new_user [0];
                        Thread client_list = new Thread 
                    (new client_listener (opts, client));
            Thread client_write = new Thread
                    (new client_writer (opts, client));
            client_list.start ();
            Thread.currentThread ().sleep (1000);
            client_write.start ();

         }catch (IOException e) {
            System.out.println(opts.username + "User exists: " + e);
            System.exit(1);
         }catch (InterruptedException e) {
            System.out.println("Client warning: " + e);
            System.exit(1);
         }
      }
   }
   
   static class writer implements Runnable {
      Socket socket;
      chatter.options opts;
      PrintWriter writer;
      writer (Socket _socket, chatter.options _opts,
                     PrintWriter _writer) {
         socket = _socket;
         opts = _opts;
         writer = _writer;
      }
      public void run () {
         writer.printf ("%s%n", opts.username);
         writer.flush ();
         writer.close ();
      }
   }
  
  static class receiver implements Runnable {
    Socket clSocket;
    Scanner scan;
    receiver(Socket socket, Scanner scanner){
      clSocket = socket;
      scan = scanner;
    }
    public void run () {
      while (! clSocket.isInputShutdown() && scan.hasNextLine() ){
        String message = scan.nextLine();
        messageList.add(message);
      }
      scan.close();
    }
  }
  
  static class sender implements Runnable {
    Socket clSocket;
    PrintWriter writer;
    String message;
    sender(Socket socket, PrintWriter write){
      clSocket = socket;
      writer = write;
    }
    public void run () {
      writer.flush();
      while(!messageList.isEmpty()){
         if(clSocket.isOutputShutdown())break;
         writer.printf("%s%n", messageList.get(0));
         messageList.remove(0);
         writer.flush();
        }
      writer.close();
    }
  }
  
 static void connect(int portno){
    int portnum = portno;
    socket = null;
    try{
      //creates new socket on portnumber
      socket = new ServerSocket(portnum);
    }
    catch (IOException e) {
      System.out.println("Could not listen on port: " +
             portnum + ", " + e);
      System.exit(1);
    }
    Socket clientSocket = null;
    //while loop to create multithreaded server
    while (true) {
      try {
        //accepts socket from client
        clientSocket = socket.accept();
      }
      catch (IOException e) {
        System.out.println("Accept failed: " + portnum + ", " + e);
        System.exit(1);
      }
      //worker threads
      try{
        Thread workerReciever = new Thread(new receiver(clientSocket,
                    new Scanner (clientSocket.getInputStream ())));
        Thread workerSender = new Thread(new sender(clientSocket,
                new PrintWriter (clientSocket.getOutputStream ())));
        workerReciever.start();
        workerSender.start();
      }
      catch (IOException e) {
        System.out.println( e );
      }
    }
  }
  
  
  static class client_listener implements Runnable {
      private String user;
      private Socket client;
      client_listener (chatter.options opts, Socket client) {
         this.user = opts.username;
         this.client = client;
      }

      public void run () {
         try {
            Scanner client_in = new Scanner (client.getInputStream ());
            usertable.add_user (user);
               while (client_in.hasNextLine ()) { 
                  String line = client_in.nextLine ();
                  line = line.trim ();
                  if (line.length () == 0) continue;
                  line = user + ": " + line;
                  put (buffer, line); 
               }
            
            usertable.set_false_user (user);  
            client_in.close ();
            client.close (); 
         }catch (IOException e) {
            System.out.println("Client warning: " + e);
            System.exit(1);
         }
      }
   }
   
   static class client_writer implements Runnable {
      private String user;
      private Socket client;
      client_writer (chatter.options opts, Socket client) {
         this.user = opts.username;
         this.client = client;
      }

      public void run () {
         try {
            PrintWriter client_out =
               new PrintWriter (client.getOutputStream ());
            usertable.add_clientWriter (user, client_out);

            // while client connected keep the outputStream alive
            while (usertable.status_user (user)) {
               Thread.currentThread ().sleep (2000);
            }

            usertable.rm_user (user);
            PrintWriter pw = usertable.rm_clientWriter (user); 
            pw.close ();
            client_out.close ();
            client.close (); 
         }catch (IOException e) {
            System.out.println("Client warning: " + e);
            System.exit(1);
         }catch (InterruptedException e) {
            System.out.println("Client warning: " + e);
            System.exit(1);
         }
      }
   }
}
