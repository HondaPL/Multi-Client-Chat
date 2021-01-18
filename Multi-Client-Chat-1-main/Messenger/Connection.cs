using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Collections.Generic;
using MessengerLogic;
using System.IO;

namespace Messenger
{
    public class Connection
    {
        public static int BufferSize = 1024;
        public static string nick;
        public static IPAddress ip = IPAddress.Parse("192.168.1.51");
        public static int port = 1234;
        public static TcpClient client = new TcpClient();
        public static List<string> users = new List<string>();
        public static bool isEnd = false;
        public static string[] fromServer = new string[30];
        public void Connect(string nickname)
        {
            nick = nickname;
            client.Connect(ip, port);
            nickname += "$Welcome$";
            byte[] buffer = Encoding.ASCII.GetBytes(nickname);
            NetworkStream ns = client.GetStream();
            Console.WriteLine(nickname);
            ns.Write(buffer, 0, buffer.Length);
            Thread thread = new Thread(o => ReceiveData((TcpClient)o));

            thread.Start(client);

        }

        static void ReceiveData(TcpClient client)
        {
            NetworkStream ns = client.GetStream();
            byte[] receivedBytes = new byte[1024];
            int byte_count;
            StringParser parser = new StringParser();

            while ((byte_count = ns.Read(receivedBytes, 0, receivedBytes.Length)) > 0)
            {
                fromServer = new string[30];
                users.Clear();
                string message = Encoding.ASCII.GetString(receivedBytes, 0, byte_count);
                fromServer = parser.Parser(message);

                if (fromServer[0] == "List")
                {
                    for (int i = 1; i < fromServer.Length; i++)
                    {
                        users.Add(fromServer[i]);
                    }
                    isEnd = true;
                }

                else
                {
                    Console.WriteLine(message);
                    Console.WriteLine(fromServer[0]);
                    Console.WriteLine(fromServer[1]);
                    FrmOnlineUsers.frm.AssignValues(fromServer[0], fromServer[1]);
                    FrmOnlineUsers.frm.MessageFromClient();
                }
            }
        }

        public List<string> Refresh()
        {
            NetworkStream ns = client.GetStream();
            string temp = nick;
            temp += "$List$";
            byte[] buffer = ASCIIEncoding.ASCII.GetBytes(temp);
            ns.Write(buffer, 0, buffer.Length);
            while (!isEnd)
            {

            }

            isEnd = false;
            return users;
        }

        public void SendMessage(string who, string msg)
        {
            NetworkStream ns = client.GetStream();
            string temp = $"{who}$Message${msg}$";
            byte[] buffer = ASCIIEncoding.ASCII.GetBytes(temp);
            ns.Write(buffer, 0, buffer.Length);
        }

        public void FileSend(string filepath, string nick, string fileName)
        {
            NetworkStream ns = client.GetStream();
            FileStream fs = new FileStream(filepath, FileMode.Open, FileAccess.Read);
            byte[] bytesToSend = new byte[fs.Length];
            int numBytesRead = fs.Read(bytesToSend, 0, bytesToSend.Length);
            int totalBytes = 0;

            //Tutaj trzeba dodać, aby pobierał nazwę pliku
            string temp = $"{nick}$File${fileName}${fs.Length}";
            byte[] buffer = ASCIIEncoding.ASCII.GetBytes(temp);
            ns.Write(buffer, 0, buffer.Length);
            Thread.Sleep(1000);
            for (int i = 0; i <= fs.Length / BufferSize; i++)
            {
                if (fs.Length - (i * BufferSize) > BufferSize)
                {
                    ns.Write(bytesToSend, i * BufferSize,
                        BufferSize);
                    totalBytes += BufferSize;
                }
                else
                {
                    ns.Write(bytesToSend, i * BufferSize,
                        (int)fs.Length - (i * BufferSize));
                    totalBytes += (int)fs.Length - (i * BufferSize);
                }
            }
            fs.Close();

        }
    }
}
