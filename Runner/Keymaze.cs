using System;
using System.Text;
using System.IO;
using System.IO.Ports;
using System.Collections;
using System.Diagnostics;
using System.Globalization;

namespace Runner
{
    public class InfoKeymaze
    {
        public string m_strName = "";
        public string m_strSerial = "";
        public string m_strUser = "";
        public byte m_bGender = 0;
        public int m_iAge = 0;
        public int m_iWeight = 0;
        public int m_iHeight = 0;
        public DateTime m_dBirthday = DateTime.Today;
    }
    
    public class ActivityKeymaze
    {
        public Int16 m_iID = -1;
        public Int16 m_iTrack = -1;
        public DateTime m_dStart = DateTime.Today;
        public DateTime m_dTime = DateTime.Today;
        public double m_dDistance = 0;
        public Int16 m_iCalories = 0;
        public double m_dMaxSpeed = 0;
        public double m_dMaxHeart = 0;
        public double m_dAvgHeart = 0;
        public ArrayList m_lstMarks = new ArrayList();
    }

    public class Keymaze
    {
        private enum eMode
        {
            eDevice,
            eDebug
        };

        private const byte CMD_PREFIX = 0x02;

        private const byte CMD_TP_DIR = 0x78;
        private const byte CMD_TP_GET_HDR = 0x80;
        private const byte CMD_TP_GET_NEXT = 0x81;
        private const byte CMD_INFO_GET = 0x85;
        private const byte ACK_TP_GET_NONE = 0x8a;

        private SerialPort m_oSerialPort = new SerialPort();

        private double m_dProgress = 0.0;
        private string m_strDirectory = "";

        private eMode m_eMode = eMode.eDevice;


        public void SetDirectory(string in_strDirectory)
        {
            m_strDirectory = in_strDirectory;
        }

        public double GetProgressValue()
        {
            return m_dProgress;
        }

        public string[] GetPortNames()
        {
            return SerialPort.GetPortNames();
        }

        public void Open(string in_strPortName, int in_iBaudRate)
        {
            Close();
            m_oSerialPort.PortName = in_strPortName;
            m_oSerialPort.BaudRate = in_iBaudRate;
            m_oSerialPort.DtrEnable = false;
            m_oSerialPort.RtsEnable = false;
            m_oSerialPort.Parity = Parity.None;
            m_oSerialPort.StopBits = StopBits.One;
            m_oSerialPort.DataBits = 8;

            m_oSerialPort.Open();

            Drain();
        }

        public void Close()
        {
            if (m_oSerialPort.IsOpen) m_oSerialPort.Close();
        }

        private Int16 Swap(Int16 number)
        {
            return (Int16)(((number >> 8) & 0xFF) + ((number << 8) & 0xFF00));
        }

        private Int32 Swap(Int32 number)
        {
            byte b0 = (byte)((number >> 24) & 0xFF);
            byte b1 = (byte)((number >> 16) & 0xFF);
            byte b2 = (byte)((number >> 8) & 0xFF);
            byte b3 = (byte)(number & 0xFF);
            return b0 + (b1 << 8) + (b2 << 16) + (b3 << 24);
        }

        private byte CalcChecksum(byte[] in_buffer)
        {
            int iCksum = 0x0;

            for (int iData = 1; iData < in_buffer.Length; iData++)
            {
                iCksum = iCksum ^ in_buffer[iData];
            }

            return (byte)iCksum;
        }

        private void Drain()
        {
            if (!m_oSerialPort.IsOpen) return;
            if (m_oSerialPort.BytesToRead <= 0) return;
            m_oSerialPort.ReadExisting();
        }

        private byte[] ReadLog(int in_iLine)
        {
            string str = null;
            try
            {
                StreamReader file = new StreamReader("D:\\Donnees\\Documents\\Programmation\\Visual C#\\Runner\\Runner\\Portmon.log");
                int iLine = -1;
                while (iLine != in_iLine)
                {
                    if (file.EndOfStream) return null;
                    str = file.ReadLine();
                    if (str.Contains("IRP_MJ_WRITE")) iLine++;
                }
                if (iLine != in_iLine) return null;

                string strHex = "";
                
                str = file.ReadLine();
                while (!str.Contains("IRP_MJ_WRITE"))
                {
                    if (file.EndOfStream) return null;
                    
                    if (str.Contains("IRP_MJ_READ"))
                    {
                        int iStart = str.IndexOf(':');
                        if (iStart >= 0)
                        {
                            str = str.Substring(iStart + 1).Trim();
                            if (str.Length > 0) strHex += " " + str;
                        }
                    }
                    str = file.ReadLine();
                }

                MemoryStream stream = new MemoryStream();
                BinaryWriter writer = new BinaryWriter(stream);
                string[] lstHex = strHex.Trim().Split(' ');
                for (int iHex = 3; iHex < lstHex.Length; iHex++)
                {
                    writer.Write(Convert.ToByte(lstHex[iHex],16));
                }

                return stream.ToArray();
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }
            return null;
        }

        private byte[] RequestDevice(byte in_bCmd, Int16 in_iTrack, byte[] in_lstAcceptedCmd)
        {
            if (!m_oSerialPort.IsOpen) return null;

            byte[] bufferRead = new byte[256];
            byte[] bufferWrite = null;
            int iRespLength = 0;

            short iLength = (short)(in_iTrack == -1 ? 1 : 5);

            MemoryStream stream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            writer.Write(CMD_PREFIX);
            writer.Write(Swap(iLength));
            writer.Write(in_bCmd);
            if (in_iTrack != -1)
            {
                writer.Write(Swap((short)1));
                writer.Write(Swap(in_iTrack));
            }
            bufferWrite = stream.ToArray();
            writer.Write(CalcChecksum(bufferWrite));

            bool bFound = true;
            bufferWrite = stream.ToArray();
            m_oSerialPort.Write(bufferWrite, 0, bufferWrite.Length);
            

            for (int i = 0; i < 4; i++)
            {
                if (m_oSerialPort.BytesToRead >= 3) break;
                System.Threading.Thread.Sleep(200);
            }
            if (m_oSerialPort.BytesToRead < 3) return null;

            int nCount = m_oSerialPort.Read(bufferRead, 0, 3);
            if (nCount < 3) return null;
            
            byte bCmd = bufferRead[0];
            iRespLength = (bufferRead[1] << 8) + bufferRead[2];

            if (bCmd != in_bCmd)
            {
                bFound = false;
                if (in_lstAcceptedCmd != null)
                {
                    for (int iAccepedCmd = 0; iAccepedCmd < in_lstAcceptedCmd.Length; iAccepedCmd++)
                    {
                        if (bCmd == in_lstAcceptedCmd[iAccepedCmd])
                        {
                            bFound = true;
                            break;
                        }
                    }
                }
            }

            if (!bFound) return null;
            if (iRespLength <= 0) return null;

            for (int i = 0; i < 4; i++)
            {
                if (m_oSerialPort.BytesToRead >= iRespLength) break;
                System.Threading.Thread.Sleep(200);
            }
            if (m_oSerialPort.BytesToRead < iRespLength) return null;

            bufferRead = new byte[iRespLength];
            m_oSerialPort.Read(bufferRead, 0, iRespLength);
            int iCksum = m_oSerialPort.ReadByte();

            return bufferRead;
        }

        public InfoKeymaze GetInformation()
        {
            byte[] buffer = null;

            if (m_eMode == eMode.eDevice)
                buffer = RequestDevice(CMD_INFO_GET, -1, null);
            else
                buffer = ReadLog(0);
            
            if (buffer == null) return null;

            InfoKeymaze info = new InfoKeymaze();

            MemoryStream stream = new MemoryStream(buffer);
            BinaryReader reader = new BinaryReader(stream);

            info.m_strName = Encoding.Default.GetString(reader.ReadBytes(12));
            reader.ReadBytes(13);
            info.m_strSerial = Encoding.Default.GetString(reader.ReadBytes(17));
            info.m_strUser = Encoding.Default.GetString(reader.ReadBytes(11));
            info.m_bGender = reader.ReadByte();
            info.m_iAge = reader.ReadByte();
            reader.ReadBytes(3);
            info.m_iWeight = reader.ReadByte();
            reader.ReadBytes(3);
            info.m_iHeight = reader.ReadByte();
            reader.ReadBytes(4);
            int y = 1792 + (int)reader.ReadByte();
            int m = 1 + (int)reader.ReadByte();
            int d = reader.ReadByte();
            info.m_dBirthday = new DateTime(y, m, d);

            return info;
        }

        public ArrayList GetActivities()
        {
            byte[] buffer = null;

            if (m_eMode == eMode.eDevice)
                buffer = RequestDevice(CMD_TP_DIR, -1, null);
            else
                buffer = ReadLog(1);

            if (buffer == null) return null;

            MemoryStream stream = new MemoryStream(buffer);
            BinaryReader reader = new BinaryReader(stream);

            ArrayList lstActivities = new ArrayList();
            while (reader.BaseStream.Position + 31 <= reader.BaseStream.Length)
            {
                ActivityKeymaze activity = new ActivityKeymaze();

                int y = 2000 + (int)reader.ReadByte();
                int m = reader.ReadByte();
                int d = reader.ReadByte();
                int h = reader.ReadByte();
                int min = reader.ReadByte();
                int s = reader.ReadByte();
                reader.ReadByte();
                double dTime = (double)Swap(reader.ReadInt32()) / 10.0;
                activity.m_dDistance = Swap(reader.ReadInt32());
                activity.m_iCalories = Swap(reader.ReadInt16());
                activity.m_dMaxSpeed = Swap(reader.ReadInt16());
                activity.m_dMaxHeart = reader.ReadByte();
                activity.m_dAvgHeart = reader.ReadByte();
                reader.ReadBytes(6);
                activity.m_iTrack = Swap(reader.ReadInt16());
                activity.m_iID = Swap(reader.ReadInt16());

                TimeZone timeZone = TimeZone.CurrentTimeZone;
                activity.m_dStart = timeZone.ToUniversalTime(new DateTime(y, m, d, h, min, s));
                activity.m_dTime = new DateTime(y, m, d, 0, 0, 0);
                activity.m_dTime = activity.m_dTime.AddSeconds(dTime);

                lstActivities.Add(activity);
            }

            return lstActivities;
        }

        public string GetTrackpoint(ActivityKeymaze in_activity)
        {
            byte[] buffer = null;

            if (m_eMode == eMode.eDevice)
                buffer = RequestDevice(CMD_TP_GET_HDR, in_activity.m_iTrack, null);
            else
                buffer = ReadLog(2);

            if (buffer == null) return "";

            MemoryStream stream = new MemoryStream(buffer);
            BinaryReader reader = new BinaryReader(stream);
            reader.ReadBytes(25);
            int nMarks = Swap(reader.ReadInt16());

            byte[] lstReq = new byte[2];
            lstReq[0] = ACK_TP_GET_NONE;
            lstReq[1] = CMD_TP_GET_HDR;

            DateTime dStart = in_activity.m_dStart;

            m_dProgress = 0.0;

            in_activity.m_lstMarks.Clear();
            int iLine = 0;
            while (in_activity.m_lstMarks.Count < nMarks)
            {
                if (m_eMode == eMode.eDevice)
                    buffer = RequestDevice(CMD_TP_GET_NEXT, -1, lstReq);
                else
                    buffer = ReadLog(3 + iLine); iLine++;

                if (buffer == null) break;

                stream = new MemoryStream(buffer);
                reader = new BinaryReader(stream);

                reader.ReadBytes(31);
                Int16 iSpeed = 0;
                while (reader.BaseStream.Position + 15 <= reader.BaseStream.Length)
                {
                    Mark oMark = new Mark();
                    oMark.m_dLati = Swap(reader.ReadInt32())/1000000.0;
                    oMark.m_dLongi = Swap(reader.ReadInt32())/1000000.0;
                    oMark.m_dElevation = Swap(reader.ReadInt16());
                    iSpeed = Swap(reader.ReadInt16());
                    if (iSpeed < 10000) oMark.m_dSpeed = iSpeed / 10.0;
                    else oMark.m_dSpeed = (iSpeed / 1000 * 100) + iSpeed % 1000;
                    oMark.m_dHeartRate = reader.ReadByte();
                    dStart = dStart.AddSeconds((double)Swap(reader.ReadInt16()) / 10.0);
                    oMark.m_dTime = dStart;

                    in_activity.m_lstMarks.Add(oMark);
                    
                    m_dProgress = 100.0 * in_activity.m_lstMarks.Count / nMarks ;
                }
            }
            m_dProgress = 100.0;
            return SaveGPX(in_activity);
        }

        private string SaveGPX(ActivityKeymaze in_activity)
        {
            if (in_activity == null) return "";

            string strFileName = m_strDirectory + "\\" + in_activity.m_dStart.ToString("yyyyMMdd") + ".gpx";

            NumberFormatInfo nfi = new NumberFormatInfo();
            nfi.NumberDecimalSeparator = ".";

            string strData = "";

            try
            {
                StreamWriter file = new StreamWriter(strFileName);
                
                file.WriteLine("<?xml version=\"1.0\"?>\n");
                file.WriteLine("<gpx version=\"1.0\" creator=\"KeyMaze 500-700 PC Software\" xmlns:st=\"urn:uuid:D0EB2ED5-49B6-44e3-B13C-CF15BE7DD7DD\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.topografix.com/GPX/1/0\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">\n");
                file.WriteLine("<extensions>\n");
                strData = "<st:activity id=\"";
                strData += in_activity.m_iID.ToString();
                strData += "\" created=\"";
                strData += in_activity.m_dStart.ToString("yyyy-MM-ddTHH:mm:ssZ");
                strData += "\" startTime=\"";
                strData += in_activity.m_dStart.ToString("yyyy-MM-ddTHH:mm:ssZ");
                strData += "\" hasStartTime=\"true\" distanceEntered=\"";
                strData += in_activity.m_dDistance.ToString("F06", nfi);
                strData += "\" timeEntered=\"";
                strData += in_activity.m_dTime.ToString("HH:mm:ss");
                strData += "\" calories=\"";
                strData += in_activity.m_iCalories.ToString();
                strData += "\">\n";
                file.WriteLine(strData);
                file.WriteLine("  <st:heartRateTrack>\n");
                int nMarks = in_activity.m_lstMarks.Count;
                for (int iMark = 0; iMark < nMarks; iMark++)
                {
                    Mark oMark = (Mark)in_activity.m_lstMarks[iMark];
                    if (oMark == null) continue;
                    int iBpm = (int)oMark.m_dHeartRate;
                    strData = "    <st:heartRate time=\"";
                    strData += oMark.m_dTime.ToString("yyyy-MM-ddTHH:mm:ssZ");
                    strData += "\" bpm=\"";
                    strData += iBpm.ToString();
                    strData += "\" />\n";
                    file.WriteLine(strData);
                }
                file.WriteLine("   </st:heartRateTrack>\n");
                file.WriteLine(" </st:activity>\n");
                file.WriteLine(" </extensions>\n");

                file.WriteLine("<trk>\n");
                strData = "<name>";
                strData += in_activity.m_iID.ToString("D3");
                strData += "-";
                strData += in_activity.m_dStart.ToString("yyyy/MM/dd-HH:mm:ss");
                strData += "</name>\n";
                file.WriteLine(strData);
                file.WriteLine("<number>1</number>\n");
                file.WriteLine("<trkseg>\n");
                for (int iMark = 0; iMark < nMarks; iMark++)
                {
                    Mark oMark = (Mark)in_activity.m_lstMarks[iMark];
                    if (oMark == null) continue;
                    strData = "<trkpt lat=\"";
                    strData += oMark.m_dLati.ToString("F06", nfi);
                    strData += "\" lon=\"";
                    strData += oMark.m_dLongi.ToString("F06", nfi);
                    strData += "\"><ele>";
                    strData += oMark.m_dElevation.ToString("F01", nfi);
                    strData += "</ele><speed>";
                    strData += oMark.m_dSpeed.ToString("F01", nfi);
                    strData += "</speed><time>";
                    strData += oMark.m_dTime.ToString("yyyy-MM-ddTHH:mm:ssZ");
                    strData += "</time>\n";
                    file.WriteLine(strData);
                    file.WriteLine("</trkpt>\n");
                }
                file.WriteLine("</trkseg>\n");
                file.WriteLine("</trk>\n");
                file.WriteLine("</gpx>");

                file.Close();
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            return strFileName;
        }
    }
}
