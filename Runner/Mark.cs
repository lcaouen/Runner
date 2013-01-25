using System;
using System.Collections.Generic;
using System.Text;

namespace Runner
{
	public class Mark
	{
        public DateTime m_dTime = DateTime.Today;
		public double m_dLati = 0.0;
        public double m_dLongi = 0.0;
        public double m_dElevation = 0.0;
        public double m_dHeartRate = 0.0;
        public double m_dSpeed = 0.0;

        public void Clear()
        {
            m_dTime = DateTime.Today;
		    m_dLati = 0.0;
            m_dLongi = 0.0;
            m_dElevation = 0.0;
            m_dHeartRate = 0.0;
            m_dSpeed = 0.0;
        }
	}
}
