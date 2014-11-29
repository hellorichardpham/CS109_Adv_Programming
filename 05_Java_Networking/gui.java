// mhamade - Mustapha Hamade

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import javax.swing.*;
import java.util.*;
import java.util.concurrent.*;

import static java.lang.String.*;
import static java.lang.System.*;

class gui {

   static class confirm implements ActionListener { 
      public void actionPerformed (ActionEvent event) {
         int response = JOptionPane.showConfirmDialog (null, "");
         if (response == 0) exit (0);
      }
   }

   static class gui_start implements Runnable {
      chatter.options opts;
      gui_start (chatter.options opts) {
         this.opts = opts;
      }

      public void run () {
         try {
            Socket socket = new Socket (opts.server_hostname,
                                        opts.server_portnumber);
            out.printf ("socket check: OK%n");

            Font courierFont = new Font ("Courier", Font.PLAIN, 14);
            Font courierBold = new Font ("Courier", Font.BOLD, 14);
            JFrame frame = new JFrame (opts.progname); 
            Container cpane = frame.getContentPane ();

            GridBagLayout layout = new GridBagLayout ();
            GridBagConstraints constraints = new GridBagConstraints ();
            constraints.anchor = GridBagConstraints.EAST;
            constraints.gridwidth = GridBagConstraints.REMAINDER;

            Button quit = new Button ("Quit");
            confirm quitlistener = new confirm ();
            quit.addActionListener (quitlistener);

            String spaces = "     ";
            cpane.add (quit);
            cpane.add (new JLabel (opts.server_hostname + ":" + 
                                  opts.server_portnumber));
            cpane.add (new JLabel (spaces + "User: " + opts.username));
            cpane.add (new JLabel (spaces));
            JLabel progname = new JLabel (spaces + opts.progname);
            layout.setConstraints (progname, constraints);
            cpane.add (progname);

            JTextArea textarea = new JTextArea (20, 60);
            textarea.setEditable (false);
            textarea.setFont (courierFont);
            JScrollPane scroll = new JScrollPane (textarea);
            constraints.anchor = GridBagConstraints.WEST;
            layout.setConstraints (scroll, constraints);

            JTextField textfield = new JTextField (60);
            textfield.setFont (courierBold);
            layout.setConstraints (textfield, constraints);

            cpane.setLayout (layout);
            cpane.add (scroll);
            cpane.add (textfield);

            copyfield textListener = new copyfield (opts, 
               new PrintWriter (socket.getOutputStream ()));
            textfield.addActionListener (textListener);

            frame.pack ();
            textfield.requestFocusInWindow ();
            frame.setLocation (256, 256);
            frame.setVisible (true);
            textarea.append ("Signing in as " +
                                     opts.username + ":\n");
            textarea.append ("You can starting chatting...\n");

            Thread sender = new Thread (textListener);
            Thread listener = new Thread (new gui_listen (textarea,
               new Scanner (socket.getInputStream ())));
            sender.start ();
            listener.start ();
         }catch (IOException e) {
           System.out.println("GUI error, " + e);
           System.exit(1);
         }
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

   static void put (BlockingQueue <String> source, String msg) {
      try {
         source.put (msg);
      }catch (InterruptedException e) {
         System.out.println("GUI warning, " + e);
         System.exit(1);
      }
   }

   static class copyfield implements ActionListener, Runnable {
      chatter.options opts;
      PrintWriter writer;
      private static BlockingQueue <String> buffer;       
      copyfield (chatter.options opts, PrintWriter writer) {
         this.opts = opts;
         this.writer = writer;
         buffer = new LinkedBlockingQueue <String> (); 
      }

      public void actionPerformed (ActionEvent event) {
         JTextField source = (JTextField) event.getSource ();
         String text = source.getText ();
         put (buffer, text);
         source.setText ("");
      }

      public void run () {
         writer.printf ("%s: %n", opts.username);
         writer.flush ();
         while (true) {
            String line = poll (buffer, 300);
            if (line == null) continue; 
            writer.printf ("%s%n", line);
            writer.flush ();
         } 
      }
   }

   static class gui_listen implements Runnable {
      JTextArea textarea;
      Scanner scanner;
      gui_listen (JTextArea init, Scanner scanner) {
         this.textarea = init;
         this.scanner = scanner;
      }

      public void run () {
         out.printf ("Starting GUI...%n");
         while (scanner.hasNextLine ()) {
            textarea.append (scanner.nextLine () + "\n");
         }   

         scanner.close ();
         out.printf ("Closing GUI...%n");
      }
   }

}


