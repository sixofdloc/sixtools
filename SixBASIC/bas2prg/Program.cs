﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SixBASIC;
using System.IO;

namespace bas2prg
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                if (args.Length < 2)
                {
                    Console.WriteLine("prg2bas");
                    Console.WriteLine("A commandline CBM Basic compiler.");
                    Console.WriteLine("usage: prg2bas basfile.bas prgfile.prg");
                    Console.WriteLine();
                }
                else
                {
                    BASIC basic = new BASIC();
                    PreProcessor pp = new PreProcessor();
                    List<string> basfile = File.ReadAllLines(args[0]).ToList();
                    BASICProgram p = pp.PreProcess(basfile);
                    byte[] bytes = basic.Tokenizer.Tokenize(p);
                    File.WriteAllBytes(args[1], bytes);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }

        }
    }
}