using System;
using System.Windows.Forms;
using MessengerLogic;
using System.Collections.Generic;

namespace Messenger
{
    public partial class FrmOnlineUsers : Form
    {
        public static FrmSend frm;
        public FrmOnlineUsers()
        {
            InitializeComponent();
        }

        private void FrmOnlineUsers_Load(object sender, EventArgs e)
        {
          
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        { 
            Connection refresh = new Connection();
            List<string> allUsers = new List<string>();
            allUsers =  refresh.Refresh();
            listView1.Items.Clear();
            foreach (var person in allUsers)
            {
                listView1.Items.Add(person, 0);
            }
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void btnChat_Click(object sender, EventArgs e)
        {
            frm = new FrmSend();
            frm.Show();
        }

        private void FrmOnlineUsers_FormClosed(object sender, FormClosedEventArgs e)
        {
            Application.Exit();
        }
    }
}
