using System;
using System.Windows.Forms;

namespace MyControl
{
    class MyControl : AxHost
    {
        public MyControl()
            : base("5747094E-84FB-47B4-BC0C-F89FB583895F")
        {
        }

        public void Run()
        {
            dynamic ax = GetOcx();
            ax.Run();
        }

        public void Stop()
        {
            dynamic ax = GetOcx();
            ax.Stop();
        }
    }

    class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();

            Form f = new Form();
            f.Text = "My control";
            f.StartPosition = FormStartPosition.CenterScreen;
            f.Width = 640;
            f.Height = 480;

            MyControl c = new MyControl();
            c.Dock = DockStyle.Fill;
            c.BeginInit();

            Button b = new Button();            
            b.Dock = DockStyle.Top;
            b.Text = "Run/Stop";

            bool running = false;
            b.Click += (s, e) =>
            {
                if (running)
                {
                    c.Stop();
                    running = false;
                }
                else
                {
                    c.Run();
                    running = true;
                }
            };

            f.Controls.Add(b);
            f.Controls.Add(c);
            f.ShowDialog();
        }
    }
}

