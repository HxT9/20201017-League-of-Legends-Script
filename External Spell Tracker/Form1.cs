using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace External_Spell_Tracker
{
    public partial class Form1 : Form
    {
        SpellTracker spellTracker;
        Thread refreshThread;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            spellTracker = new SpellTracker();

            loadImages();

            this.FormClosing += Form1_FormClosing;

            refreshThread = new Thread(loop_refresh);
            refreshThread.Start();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            refreshThread.Abort();
        }

        private void loadImages()
        {
            DDragonApi api = new DDragonApi();

            foreach (SpellTracker.Champion c in spellTracker.champions)
            {
                HeroEntry entry = new HeroEntry(api, c.Name);
                c.heroEntry = entry;

                flowLayoutPanel1.Invoke((MethodInvoker)(() => flowLayoutPanel1.Controls.Add(entry)));
            }
        }

        private void loop_refresh()
        {
            while (true)
            {
                spellTracker.Refresh();
                Thread.Sleep(100);
            }
        }
    }
}
