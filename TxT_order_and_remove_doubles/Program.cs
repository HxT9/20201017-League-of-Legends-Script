using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace TxT_order_and_remove_doubles
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 0 || !File.Exists(args[0]))
            {
                Console.WriteLine("File not found/specified");
                return;
            }

            StreamReader sr = new StreamReader(args[0]);
            StreamWriter sw;
            List<string> lines = new List<string>();

            while (!sr.EndOfStream)
                lines.Add(sr.ReadLine());
            sr.Close();

            lines = lines.Distinct().OrderBy(s => s).ToList();

            sw = new StreamWriter(args[0], false);
            foreach (string s in lines)
                sw.WriteLine(s);

            sw.Close();
        }
    }
}
