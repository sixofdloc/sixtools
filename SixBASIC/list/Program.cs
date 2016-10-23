using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SixBASIC;

namespace list
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Usage:  list filename.prg");
            }
            else
            {
                BASIC basic = new BASIC();
                basic.Load(args[1]);
                List<string> slist = basic.List();
                foreach (string s in slist)
                {
                    Console.WriteLine(s);
                }
                Console.WriteLine("Press Any Key To Continue");
                Console.ReadLine();
            }


        }
    }
}
