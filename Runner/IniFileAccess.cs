using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace IniFile
{
    public class IniFileAccess
    {
        [DllImport("kernel32", SetLastError = true)]
        private static extern int WritePrivateProfileString(string in_strSection, string in_strKey, string in_strValue, string pFile);
        [DllImport("kernel32", SetLastError = true)]
        private static extern int GetPrivateProfileString(string in_strSection, string in_strKey, string pDefault, byte[] prReturn, int pBufferLen, string pFile);

        private string m_strFileName;
        private byte[] m_buffer;


        public IniFileAccess(string in_strFileName)
        {
            m_strFileName = in_strFileName;
            m_buffer = new byte[256];
        }

        public string ReadValue(string in_strSection, string in_strKey)
        {
            return ReadIni(in_strSection, in_strKey);
        }

        public void WriteValue(string in_strSection, string in_strKey, string in_strValue)
        {
            WritePrivateProfileString(in_strSection, in_strKey, in_strValue, m_strFileName);
        }

        public void RemoveValue(string in_strSection, string in_strKey)
        {
            WritePrivateProfileString(in_strSection, in_strKey, null, m_strFileName);
        }

        public String[] ReadKeys(string in_strSection)
        {
            return ReadIni(in_strSection, null).Split((char)0);
        }

        public String[] ReadSections()
        {
            return ReadIni(null, null).Split((char)0);
        }

        public void RemoveSection(string in_strSection)
        {
            WritePrivateProfileString(in_strSection, null, null, m_strFileName);
        }

        private string ReadIni(string in_strSection, string in_strKey)
        {
            bool bContinue = true;
            int iBufferLength = 0; 
            string strBuffer = "";
            while (bContinue)
            {
                iBufferLength = GetPrivateProfileString(in_strSection, in_strKey, null, m_buffer, m_buffer.Length, m_strFileName);
                if (iBufferLength < m_buffer.Length - 2) bContinue = false;
                else m_buffer = new byte [m_buffer.Length * 2];
            }
            strBuffer = Encoding.GetEncoding(1252).GetString(m_buffer, 0, iBufferLength).TrimEnd((char)0);
            return (strBuffer);
        }
    }
}
