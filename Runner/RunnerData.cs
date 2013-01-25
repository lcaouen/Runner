using System;
using System.Collections.Generic;
using System.Text;

namespace Runner
{
    public class RunnerData : IComparable 
    {
        public enum eRunnerData {
            eUndefined,
            eRS200,
            eKM700,
            eFR305
        }

        public eRunnerData m_eType = eRunnerData.eUndefined;
        public string m_strFileName = "";
        public DateTime m_dDate = DateTime.Today;
        public DateTime m_dTime = DateTime.Today;
        public double m_dDistance = 0.0;
        public double m_dSpeed = 0.0;
        public double m_dAvgHR = 0.0;
        public double m_dMaxHR = 0.0;
        public double m_dCalories = 0.0;

		public int CompareTo(object obj)
		{
			if (obj is RunnerData)
			{
				RunnerData rs200 = (RunnerData)obj;
				if (this.m_dDate.ToOADate() > rs200.m_dDate.ToOADate()) return 1;
				else if (this.m_dDate.ToOADate() < rs200.m_dDate.ToOADate()) return -1;
				return 0;
			}
			else
			{
                throw new ArgumentException("object is not a RunnerData");
			}
		}
	}
}
