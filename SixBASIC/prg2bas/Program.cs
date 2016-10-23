using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SixBASIC;
using System.IO;

namespace prg2bas
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 2)
            {
            }
            else
            {
                BASIC basic = new BASIC();

                if (basic.Load(args[0]))
                {
                    List<string> slist = basic.List();
                    File.WriteAllLines(args[1], slist);
                }
                else
                {
                    Console.WriteLine("Load error: " + basic.LastError);
                }
                //Console.ReadLine();
            }

        }
    }
}
