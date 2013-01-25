using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Runner
{
    class RS200
    {
        [DllImport("RS200.dll", SetLastError = true)]
        public static extern string DecodeRS200(string in_strFileName);

        [DllImport("RS200.dll", SetLastError = true)]
        public static extern double GetRS200ProgressValue();

        [DllImport("RS200.dll", SetLastError = true)]
        public static extern string GetRS200Satus();

        [DllImport("RS200.dll", SetLastError = true)]
        public static extern double GetRS200LevelValue();

        public string Decode(string in_strFileName)
        {
            return DecodeRS200(in_strFileName);
        }

        public double GetProgressValue()
        {
            return GetRS200ProgressValue() * 100.0;
        }

        public string GetStatus()
        {
            return GetRS200Satus();
        }

        public double GetLevelValue()
        {
            return GetRS200LevelValue() * 100.0;
        }
    }
}
