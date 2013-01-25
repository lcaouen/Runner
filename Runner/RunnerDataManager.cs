using System;
using System.Collections;
using System.Xml;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Globalization;

namespace Runner
{
    public class RunnerDataManager
    {
        private XmlDocument m_XmlDoc = new XmlDocument();
        
        private ArrayList m_lstData = new ArrayList();
        private ArrayList m_lstMarks = new ArrayList();
        private int m_nMarks = 0;

        private int m_iSel = -1;

        public int GetRunnerDataCount()
        {
            return m_lstData.Count;
        }

        public RunnerData GetRunnerData(int in_iData)
        {
            if (in_iData < 0 || in_iData > m_lstData.Count) return null;
            return (RunnerData) m_lstData[in_iData];
        }

        public int GetMarkCount()
        {
            return m_nMarks;
        }

        public Mark GetMark(int in_iMark)
        {
            if (in_iMark < 0 || in_iMark > m_lstMarks.Count) return null;
            return (Mark)m_lstMarks[in_iMark];
        }

        public void LoadRunnerData(string in_strDirectory)
        {
            m_lstData.Clear();

            Directory.CreateDirectory(in_strDirectory);
            
            string[] lstFiles = Directory.GetFiles(in_strDirectory, "*.xml");
            for (int iFile = 0; iFile < lstFiles.Length; iFile++)
            {
                LoadXML(lstFiles[iFile]);
            }
            
            lstFiles = Directory.GetFiles(in_strDirectory, "*.gpx");
            for (int iFile = 0; iFile < lstFiles.Length; iFile++)
            {
                LoadGPX(lstFiles[iFile]);
            }

            lstFiles = Directory.GetFiles(in_strDirectory, "*.tcx");
            for (int iFile = 0; iFile < lstFiles.Length; iFile++)
            {
                LoadTCX(lstFiles[iFile]);
            }

            m_lstData.Sort();
            m_lstData.Reverse();
        }

        public object[] LoadMarks(int in_iData)
        {
            RunnerData oData = GetRunnerData(in_iData);
            if (oData == null) return null;

            m_iSel = in_iData;

            if (oData.m_eType == RunnerData.eRunnerData.eRS200)
                return LoadKMLMarks(oData.m_strFileName);
            else if (oData.m_eType == RunnerData.eRunnerData.eKM700)
                return LoadGPXMarks(oData.m_strFileName);
            else if (oData.m_eType == RunnerData.eRunnerData.eFR305)
                return LoadTCXMarks(oData.m_strFileName);
            else
                return null;
        }

        public void SaveMarks()
        {
            RunnerData oData = GetRunnerData(m_iSel);
            if (oData == null) return;
            if (oData.m_eType != RunnerData.eRunnerData.eRS200) return;
            
            SaveKMLMarks(oData.m_strFileName);
        }

        public void AddMark(double in_dLongi, double in_dLati)
        {
            RunnerData oData = GetRunnerData(m_iSel);
            if (oData == null) return;
            if (oData.m_eType != RunnerData.eRunnerData.eRS200) return;
            
            Mark oLatLong = null;

            if (m_nMarks < m_lstMarks.Count)
                oLatLong = (Mark)m_lstMarks[m_nMarks];

            if (oLatLong == null)
            {
                oLatLong = new Mark();
                m_lstMarks.Add(oLatLong);
            }

            m_nMarks++;
            oLatLong.m_dLongi = in_dLongi;
            oLatLong.m_dLati = in_dLati;
        }

        public void UndoMark()
        {
            RunnerData oData = GetRunnerData(m_iSel);
            if (oData == null) return;
            if (oData.m_eType != RunnerData.eRunnerData.eRS200) return;
            
            m_nMarks--;
            if (m_nMarks > m_lstMarks.Count) m_nMarks = m_lstMarks.Count;
            if (m_nMarks < 0) m_nMarks = 0;
        }

        public void RedoMark()
        {
            RunnerData oData = GetRunnerData(m_iSel);
            if (oData == null) return;
            if (oData.m_eType != RunnerData.eRunnerData.eRS200) return;
            
            m_nMarks++;
            if (m_nMarks > m_lstMarks.Count) m_nMarks = m_lstMarks.Count;
            if (m_nMarks < 0) m_nMarks = 0;
        }

        private void LoadGPX(string in_strFileName)
        {
            if (in_strFileName == null) return;
            
            RunnerData data = new RunnerData();
            data.m_strFileName = in_strFileName;
            data.m_eType = RunnerData.eRunnerData.eKM700;

            try
            {
                StreamReader file = new StreamReader(data.m_strFileName);
                string strBuffer = file.ReadToEnd();
                file.Close();

                strBuffer = strBuffer.Replace(" version=\"1.0\" creator=\"KeyMaze 500-700 PC Software\" xmlns:st=\"urn:uuid:D0EB2ED5-49B6-44e3-B13C-CF15BE7DD7DD\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.topografix.com/GPX/1/0\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\"", "");
                strBuffer = strBuffer.Replace("st:", "");
                m_XmlDoc.LoadXml(strBuffer);

                string strPath = "//gpx/extensions/activity/";

                data.m_dDistance = ReadNodeToDouble(strPath + "@distanceEntered") / 1000.0;
                data.m_dCalories = ReadNodeToDouble(strPath + "@calories");
                data.m_dDate = ReadNodeToDateTime(strPath + "@startTime", "yyyy\\-MM\\-dd\\THH\\:mm\\:ss\\Z", true);
                data.m_dTime = ReadNodeToDateTime(strPath + "@timeEntered", "HH\\:mm\\:ss", false);
                double dTime = data.m_dTime.Hour * 3600 + data.m_dTime.Minute * 60 + data.m_dTime.Second;
                if (dTime != 0.0) data.m_dSpeed = data.m_dDistance * 3600 / dTime;

                data.m_dMaxHR = 0.0;
                data.m_dAvgHR = 0.0;

                string[] lstHR = ReadNodes(strPath + "heartRateTrack/heartRate/@bpm");
                if (lstHR != null)
                {
                    int nHR = lstHR.Length;
                    for (int iHR = 0; iHR < nHR; iHR++)
                    {
                        double dValue = ParseToDouble(lstHR[iHR]);
                        if (dValue > data.m_dMaxHR) data.m_dMaxHR = dValue;
                        data.m_dAvgHR += dValue;
                    }
                    if (nHR > 0) data.m_dAvgHR /= nHR;
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }
            m_lstData.Add(data);
        }

        private void LoadTCX(string in_strFileName)
        {
            if (in_strFileName == null) return;

            RunnerData data = new RunnerData();
            data.m_strFileName = in_strFileName;
            data.m_eType = RunnerData.eRunnerData.eFR305;
            data.m_dTime = DateTime.Today;
            data.m_dDistance = 0.0;
            data.m_dCalories = 0.0;
            data.m_dMaxHR = 0.0;
            data.m_dAvgHR = 0.0;
            data.m_dDate = DateTime.Today;

            try
            {
                StreamReader file = new StreamReader(data.m_strFileName);
                string strBuffer = file.ReadToEnd();
                file.Close();

                strBuffer = strBuffer.Replace(" xmlns=\"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd\"", "");
                strBuffer = strBuffer.Replace("xsi:", "");
                m_XmlDoc.LoadXml(strBuffer);

                string strPath = "//TrainingCenterDatabase/Activities/Activity/Lap";

                XmlNodeList xmlNodes = m_XmlDoc.DocumentElement.SelectNodes(strPath);
                if (xmlNodes == null) return;


                double dTime = 0.0;
                int nNodes = xmlNodes.Count;
                for (int iNode = 0; iNode < nNodes; iNode++)
                {
                    data.m_dDistance += ReadNodeToDouble(xmlNodes[iNode], "DistanceMeters") / 1000.0;
                    data.m_dCalories += ReadNodeToDouble(xmlNodes[iNode], "Calories");
                    double dMaxHR = ReadNodeToDouble(xmlNodes[iNode], "MaximumHeartRateBpm/Value");
                    if (dMaxHR > data.m_dMaxHR) data.m_dMaxHR = dMaxHR;
                    data.m_dAvgHR += ReadNodeToDouble(xmlNodes[iNode], "AverageHeartRateBpm/Value");
                    if (iNode == 0) data.m_dDate = ReadNodeToDateTime(xmlNodes[iNode], "@StartTime", "yyyy\\-MM\\-dd\\THH\\:mm\\:ss\\Z", true);
                    string strTime = ReadNode(xmlNodes[iNode], "TotalTimeSeconds");
                    dTime += ParseToDouble(strTime);
                }
                if (nNodes > 0) data.m_dAvgHR /= nNodes;
                data.m_dTime = data.m_dTime.AddSeconds(dTime);
                if (dTime != 0.0) data.m_dSpeed += data.m_dDistance * 3600.0 / dTime;
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }
            m_lstData.Add(data);
        }

        private void LoadXML(string in_strFileName)
        {
            if (in_strFileName == null) return;
            
            RunnerData data = new RunnerData();
            data.m_strFileName = in_strFileName;
            data.m_eType = RunnerData.eRunnerData.eRS200;

            try
            {
                FileStream fs = new FileStream(data.m_strFileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                m_XmlDoc.Load(fs);
                fs.Close();

                string strPath = "//rs200_session/session_data/";

                data.m_dDistance = ReadNodeToDouble(strPath + "summary/total_distance") / 1000.0;
                data.m_dAvgHR = ReadNodeToDouble(strPath + "summary/avg_hr");
                data.m_dMaxHR = ReadNodeToDouble(strPath + "summary/max_hr");
                data.m_dCalories = ReadNodeToDouble(strPath + "summary/calories");

                int iDay = ReadNodeToInt(strPath + "day");
                int iMonth = ReadNodeToInt(strPath + "month");
                int iYear = ReadNodeToInt(strPath + "year");
                int iHour = ReadNodeToInt(strPath + "start_hour");
                int iMinute = ReadNodeToInt(strPath + "start_minute");
                int iSecond = ReadNodeToInt(strPath + "start_second");

                data.m_dDate = new DateTime(iYear, iMonth, iDay, iHour, iMinute, iSecond);
                double dTime = ReadNodeToDouble(strPath + "summary/length");
                data.m_dTime = DateTime.Today.AddSeconds(dTime);

                if (dTime != 0.0) data.m_dSpeed = data.m_dDistance * 3600 / dTime;
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }
            m_lstData.Add(data);
        }

        private object[] LoadGPXMarks(string in_strFileName)
        {
            if (in_strFileName == null) return null;

            m_lstMarks.Clear();
            m_nMarks = 0;

            try
            {
                StreamReader file = new StreamReader(in_strFileName);
                string strBuffer = file.ReadToEnd();
                file.Close();

                strBuffer = strBuffer.Replace(" version=\"1.0\" creator=\"KeyMaze 500-700 PC Software\" xmlns:st=\"urn:uuid:D0EB2ED5-49B6-44e3-B13C-CF15BE7DD7DD\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.topografix.com/GPX/1/0\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\"", "");
                strBuffer = strBuffer.Replace("st:", "");
                m_XmlDoc.LoadXml(strBuffer);

                Mark oMarkMin = new Mark();
                Mark oMarkMax = new Mark();
                oMarkMin.m_dLati = oMarkMin.m_dLongi = double.MaxValue;
                oMarkMax.m_dLati = oMarkMax.m_dLongi = double.MinValue;

                string strPath = "//gpx/trk/trkseg/trkpt";
                XmlNodeList xmlNodes = m_XmlDoc.DocumentElement.SelectNodes(strPath);
                if (xmlNodes == null) return null;

                string[] lstHR = ReadNodes("//gpx/extensions/activity/heartRateTrack/heartRate/@bpm");

                int nNodes = xmlNodes.Count;
                for (int iNode = 0; iNode < nNodes; iNode++)
                {
                    m_nMarks++;

                    Mark oMark = new Mark();
                    oMark.m_dLongi = ReadNodeToDouble(xmlNodes[iNode], "@lon");
                    oMark.m_dLati = ReadNodeToDouble(xmlNodes[iNode], "@lat");
                    oMark.m_dElevation = ReadNodeToDouble(xmlNodes[iNode], "ele");
                    oMark.m_dSpeed = ReadNodeToDouble(xmlNodes[iNode], "speed") / 10.0;
                    string strTime = ReadNode(xmlNodes[iNode], "time");
                    oMark.m_dTime = ParseToDateTime(strTime, "yyyy\\-MM\\-dd\\THH\\:mm\\:ss\\Z", true);
                    oMark.m_dHeartRate = ParseToDouble(lstHR[iNode]);

                    m_lstMarks.Add(oMark);

                    if (oMark.m_dLongi < oMarkMin.m_dLongi) oMarkMin.m_dLongi = oMark.m_dLongi;
                    if (oMark.m_dLati < oMarkMin.m_dLati) oMarkMin.m_dLati = oMark.m_dLati;
                    if (oMark.m_dLongi > oMarkMax.m_dLongi) oMarkMax.m_dLongi = oMark.m_dLongi;
                    if (oMark.m_dLati > oMarkMax.m_dLati) oMarkMax.m_dLati = oMark.m_dLati;
                }

                if (nNodes > 1)
                {
                    object[] oCoordinates = new object[2];
                    oCoordinates[0] = (oMarkMax.m_dLongi + oMarkMin.m_dLongi) / 2.0;
                    oCoordinates[1] = (oMarkMax.m_dLati + oMarkMin.m_dLati) / 2.0;
                    return oCoordinates;
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            return null;
        }

        private object[] LoadTCXMarks(string in_strFileName)
        {
            if (in_strFileName == null) return null;

            m_lstMarks.Clear();
            m_nMarks = 0;

            try
            {
                StreamReader file = new StreamReader(in_strFileName);
                string strBuffer = file.ReadToEnd();
                file.Close();

                strBuffer = strBuffer.Replace(" xmlns=\"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd\"", "");
                strBuffer = strBuffer.Replace("xsi:", "");
                m_XmlDoc.LoadXml(strBuffer);

                Mark oMarkMin = new Mark();
                Mark oMarkMax = new Mark();
                oMarkMin.m_dLati = oMarkMin.m_dLongi = double.MaxValue;
                oMarkMax.m_dLati = oMarkMax.m_dLongi = double.MinValue;

                string strPath = "//TrainingCenterDatabase/Activities/Activity/Lap/";
                DateTime dPrevTime = ReadNodeToDateTime(strPath + "@StartTime", "yyyy\\-MM\\-dd\\THH\\:mm\\:ss\\Z", true);
                double dPrevDist = 0.0;

                strPath = "//TrainingCenterDatabase/Activities/Activity/Lap/Track/Trackpoint";
                XmlNodeList xmlNodes = m_XmlDoc.DocumentElement.SelectNodes(strPath);
                if (xmlNodes == null) return null;

                int nNodes = xmlNodes.Count;
                for (int iNode = 0; iNode < nNodes; iNode++)
                {
                    Mark oMark = new Mark();
                    oMark.m_dLongi = ReadNodeToDouble(xmlNodes[iNode], "Position/LongitudeDegrees");
                    if (oMark.m_dLongi == 0.0) continue;
                    oMark.m_dLati = ReadNodeToDouble(xmlNodes[iNode], "Position/LatitudeDegrees");
                    if (oMark.m_dLati == 0.0) continue;
                    oMark.m_dElevation = ReadNodeToDouble(xmlNodes[iNode], "AltitudeMeters");
                    string strTime = ReadNode(xmlNodes[iNode], "Time");
                    oMark.m_dTime = ParseToDateTime(strTime, "yyyy\\-MM\\-dd\\THH\\:mm\\:ss\\Z", true);
                    oMark.m_dHeartRate = ReadNodeToDouble(xmlNodes[iNode], "HeartRateBpm/Value");
                    double dDistance = ReadNodeToDouble(xmlNodes[iNode], "DistanceMeters") / 1000.0;
                    TimeSpan oTimeSpan = oMark.m_dTime - dPrevTime;
                    if (oTimeSpan.TotalHours > 0)
                        oMark.m_dSpeed = (dDistance - dPrevDist) / (oTimeSpan.TotalHours);
                    dPrevTime = oMark.m_dTime;
                    dPrevDist = dDistance;

                    m_lstMarks.Add(oMark);

                    if (oMark.m_dLongi < oMarkMin.m_dLongi) oMarkMin.m_dLongi = oMark.m_dLongi;
                    if (oMark.m_dLati < oMarkMin.m_dLati) oMarkMin.m_dLati = oMark.m_dLati;
                    if (oMark.m_dLongi > oMarkMax.m_dLongi) oMarkMax.m_dLongi = oMark.m_dLongi;
                    if (oMark.m_dLati > oMarkMax.m_dLati) oMarkMax.m_dLati = oMark.m_dLati;
                    m_nMarks++;
                }

                if (nNodes > 1)
                {
                    object[] oCoordinates = new object[2];
                    oCoordinates[0] = (oMarkMax.m_dLongi + oMarkMin.m_dLongi) / 2.0;
                    oCoordinates[1] = (oMarkMax.m_dLati + oMarkMin.m_dLati) / 2.0;
                    return oCoordinates;
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            return null;
        }

        private object[] LoadKMLMarks(string in_strFileName)
        {
            if (in_strFileName == null) return null;
            string strFileName = in_strFileName.Replace(".xml", ".kml");
            
            m_lstMarks.Clear();
            m_nMarks = 0;

            string strBuffer = "";

            try
            {
                StreamReader file = new StreamReader(strFileName);
                strBuffer = file.ReadToEnd();
                file.Close();
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            string strStart = "<coordinates>";
            string strEnd = "</coordinates>";
            int iStart = strBuffer.IndexOf(strStart);
            int iEnd = strBuffer.IndexOf(strEnd);

            if (iStart < 0 || iEnd < 0) return null;

            iStart += strStart.Length;
            string strPts = strBuffer.Substring(iStart, iEnd - iStart);
            string[] lstSep = { ",0 " };
            string[] lstMarks = strPts.Split(lstSep, StringSplitOptions.None);

            Mark oMarkMin = new Mark();
            Mark oMarkMax = new Mark();
            oMarkMin.m_dLati = oMarkMin.m_dLongi = double.MaxValue;
            oMarkMax.m_dLati = oMarkMax.m_dLongi = double.MinValue;

            for (int iPt = 0; iPt < lstMarks.Length; iPt++)
            {
                string[] lstPts = lstMarks[iPt].Split(',');
                if (lstPts.Length != 2) continue;
                m_nMarks++;

                Mark oMark = new Mark();
                oMark.m_dLongi = ParseToDouble(lstPts[0]);
                oMark.m_dLati = ParseToDouble(lstPts[1]);

                m_lstMarks.Add(oMark);

                if (oMark.m_dLongi < oMarkMin.m_dLongi) oMarkMin.m_dLongi = oMark.m_dLongi;
                if (oMark.m_dLati < oMarkMin.m_dLati) oMarkMin.m_dLati = oMark.m_dLati;
                if (oMark.m_dLongi > oMarkMax.m_dLongi) oMarkMax.m_dLongi = oMark.m_dLongi;
                if (oMark.m_dLati > oMarkMax.m_dLati) oMarkMax.m_dLati = oMark.m_dLati;
            }

            if (lstMarks.Length > 1)
            {
                object[] oCoordinates = new object[2];
                oCoordinates[0] = (oMarkMax.m_dLongi + oMarkMin.m_dLongi) / 2.0;
                oCoordinates[1] = (oMarkMax.m_dLati + oMarkMin.m_dLati) / 2.0;
                return oCoordinates;
            }

            return null;
        }

        private void SaveKMLMarks(string in_strFileName)
        {
            if (in_strFileName == null) return;

            string strFileName = in_strFileName.Replace(".xml", ".kml");

            NumberFormatInfo nfi = new NumberFormatInfo();
            nfi.NumberDecimalSeparator = ".";

            String strKmlPoints = "";

            if (m_nMarks > m_lstMarks.Count) m_nMarks = m_lstMarks.Count;

            for (int iMark = 0; iMark < m_nMarks; iMark++)
            {
                Mark oLatLong = (Mark)m_lstMarks[iMark];
                if (oLatLong == null) continue;
                string strLongi = oLatLong.m_dLongi.ToString(nfi);
                string strLati = oLatLong.m_dLati.ToString(nfi);
                strKmlPoints += strLongi + "," + strLati + ",0 ";
            }

            try
            {
                StreamWriter file = new StreamWriter(strFileName);
                file.WriteLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
                file.WriteLine("<kml xmlns=\"http://earth.google.com/kml/2.2\">");
                file.WriteLine("<Document>");
                file.WriteLine("<name>Itinéraire</name>");
                file.WriteLine("<Style id=\"roadStyle\">");
                file.WriteLine("<LineStyle>");
                file.WriteLine("<color>7fcf0064</color>");
                file.WriteLine("<width>6</width>");
                file.WriteLine("</LineStyle>");
                file.WriteLine("</Style>");
                file.WriteLine("<Placemark>");
                file.WriteLine("<styleUrl>#roadStyle</styleUrl>");
                file.WriteLine("<MultiGeometry>");
                file.WriteLine("<LineString>");
                file.WriteLine("<coordinates>");
                file.WriteLine(strKmlPoints);
                file.WriteLine("</coordinates>");
                file.WriteLine("</LineString>");
                file.WriteLine("</MultiGeometry>");
                file.WriteLine("</Placemark>");
                file.WriteLine("</Document>");
                file.WriteLine("</kml>");
                file.Close();
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }
        }

        private string ReadNode(XmlNode in_oNode, string in_strNode)
        {
            if (in_oNode == null) in_oNode = m_XmlDoc.DocumentElement;

            XmlNodeList xmlNodes = in_oNode.SelectNodes(in_strNode);
            if (xmlNodes == null || xmlNodes.Count <= 0) return null;

            return xmlNodes[0].InnerText;
        }

        private string[] ReadNodes(string in_strNode)
        {
            XmlNodeList xmlNodes = m_XmlDoc.DocumentElement.SelectNodes(in_strNode);
            int nNodes = xmlNodes.Count;

            if (xmlNodes == null || nNodes <= 0) return null;

            string[] lstNodes = new string[nNodes];

            for (int iNode = 0; iNode < nNodes; iNode++)
                lstNodes[iNode] = xmlNodes[iNode].InnerText;

            return lstNodes;
        }

        private double ReadNodeToDouble(string in_strNode)
        {
            return ParseToDouble(ReadNode(null, in_strNode));
        }

        private int ReadNodeToInt(string in_strNode)
        {
            return ParseToInt(ReadNode(null, in_strNode));
        }

        private DateTime ReadNodeToDateTime(string in_strNode, string in_strFormat, bool in_bFullDate)
        {
            return ParseToDateTime(ReadNode(null, in_strNode), in_strFormat, in_bFullDate);
        }

        private double ReadNodeToDouble(XmlNode in_oNode, string in_strNode)
        {
            return ParseToDouble(ReadNode(in_oNode, in_strNode));
        }

        private int ReadNodeToInt(XmlNode in_oNode, string in_strNode)
        {
            return ParseToInt(ReadNode(in_oNode, in_strNode));
        }

        private DateTime ReadNodeToDateTime(XmlNode in_oNode, string in_strNode, string in_strFormat, bool in_bFullDate)
        {
            return ParseToDateTime(ReadNode(in_oNode, in_strNode), in_strFormat, in_bFullDate);
        }

        private double ParseToDouble(string in_strValue)
        {
            double dRes = 0.0;

            if (in_strValue == null) return dRes;

            try
            {
                NumberFormatInfo nfi = new NumberFormatInfo();
                nfi.NumberDecimalSeparator = ".";
                dRes = double.Parse(in_strValue, NumberStyles.Number, nfi);
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            return dRes;
        }

        private int ParseToInt(string in_strValue)
        {
            int iRes = 0;

            if (in_strValue == null) return iRes;
            
            try
            {
                iRes = int.Parse(in_strValue);
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            return iRes;
        }

        private DateTime ParseToDateTime(string in_strValue, string in_strFormat, bool in_bFullDate)
        {
            DateTime dateRes = DateTime.Today;

            if (in_strValue == null) return DateTime.Today;
            
            try
            {
                dateRes = DateTime.ParseExact(in_strValue, in_strFormat, null);
                if (in_bFullDate)
                {
                    TimeZone timeZone = TimeZone.CurrentTimeZone;
                    dateRes = timeZone.ToLocalTime(dateRes);
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.ToString());
            }

            return dateRes;
        }
    }
}
