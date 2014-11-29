// mhamade - Mustapha Hamade
import java.io.*;
import java.net.*;
import java.util.*;
import static java.lang.System.*;
class chatter {
  
  static class options {
    String traceflags;
    boolean client_is_gui = false;
    boolean is_server = true;
    String server_hostname;
    int server_portnumber;
    String username;
    final String progname = "chatter";
    options (String[] args) {
         try {
            if (args.length < 1 || args.length > 4)
                throw new NumberFormatException();
            for (String arg: args) {
                if (arg.equals("-g")) {
                    client_is_gui = true;
                    is_server = false;
                }
                else if (arg.contains("-@")) {
                    traceflags = arg;
                }
                else if (arg.contains(":")) {
                    String[] hostname_prt = arg.split (":", 2);
                    server_hostname = hostname_prt[0];
                    server_portnumber = Integer.parseInt 
                                            (hostname_prt[1]);
                    is_server = false;
                }
                else {
                    if (is_server == false) break;
                    server_portnumber = Integer.parseInt (arg);
                    is_server = true;
                }
            }
        username = args [args.length - 1];
        } catch (NumberFormatException e) {
            System.out.println("Usage: chatter [-g] [-@flags]" +
                    " [hostname:]port username");
            System.exit(1);
        }
    }
  }
  
  public static void main (String[] args) {
    options opts = new options (args);
    if (opts.is_server == true) {;
        Thread server = new Thread (new server.server_listen (opts));
        server.start ();
    }else if (opts.client_is_gui == true) {
        Thread gui_client = new Thread (new gui.gui_start (opts));
        gui_client.start ();
    }else if (opts.client_is_gui == false) {
        Thread client = new Thread (new client.client_start (opts));
        client.start ();
      }
   }
}
