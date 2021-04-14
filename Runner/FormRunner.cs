using System;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.IO.Ports;
using System.Xml;
using System.Diagnostics;
using System.Collections;
using System.Drawing;
using System.Globalization;
using System.Runtime.InteropServices;
using ZedGraph;
using IniFile;
using Microsoft.Win32;

namespace Runner
{
	[System.Runtime.InteropServices.ComVisible(true)]
	public partial class FormRunner : Form
    {
        private enum EImportMode
        {
            eRS200,
            eKM700
        }

        private readonly IniFileAccess m_oIniFileAccess = null;
        private readonly RunnerDataManager m_oRdm = new RunnerDataManager();

        private readonly string m_strDirectory = "";
        private readonly string m_strDataDirectory = "";
        private string m_strDataFile = "";

        private EImportMode m_eImport = EImportMode.eRS200;
        private Thread m_oThreadImport = null;
        private readonly RS200 m_oRS200 = new RS200();
        private readonly Keymaze m_oKeymaze = new Keymaze();
        private ArrayList m_lstActivities = null;
        private int[] m_lstIndices = null;
        private int m_iIndice = -1;

        
        public FormRunner()
        {
            InitializeComponent();
            m_strDirectory = Environment.GetCommandLineArgs()[0];
            m_strDirectory = m_strDirectory.Substring(0, m_strDirectory.LastIndexOf('\\')) + "\\";
            m_oIniFileAccess = new IniFileAccess(m_strDirectory + "config.ini");
            m_strDataDirectory = m_oIniFileAccess.ReadValue("PARAMETERS", "DATA_DIRECTORY");
            if (m_strDataDirectory == "") m_strDataDirectory = m_strDirectory + "data";
			
			webBrowserMap.ObjectForScripting = this; 
			webBrowserMap.Url = new Uri("file://" + m_strDirectory + "map.html");

            EnsureBrowserEmulationEnabled();
		}

        public void EnsureBrowserEmulationEnabled()
        {

            try
            {
                using (
                    RegistryKey rk = Registry.CurrentUser.OpenSubKey(
                            @"SOFTWARE\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BROWSER_EMULATION", true)
                )
                {
                    string exeName = AppDomain.CurrentDomain.FriendlyName;
                    object value = rk.GetValue(exeName);
                    if (value == null)
                        rk.SetValue(exeName, (uint)11001, RegistryValueKind.DWord);
                }
            }
            catch
            {
            }
        }

        private void WorkThreadRS200()
        {
            m_strDataFile = m_oRS200.Decode(m_strDataDirectory);
        }

        private void WorkThreadKeymaze()
        {
            if (m_lstIndices != null)
            {
                for (m_iIndice = 0; m_iIndice < m_lstIndices.Length; m_iIndice++)
                    m_strDataFile = m_oKeymaze.GetTrackpoint((ActivityKeymaze)m_lstActivities[m_lstIndices[m_iIndice]]);
            }
        }

        private void ButtonImportRS200_Click(object sender, EventArgs e)
        {
            m_eImport = EImportMode.eRS200;
            buttonRS200Import.Enabled = false;

            m_oThreadImport = new Thread(new ThreadStart(WorkThreadRS200))
            {
                Name = "ThreadDecoding"
            };
            m_oThreadImport.Start();

            timerThread.Start();
        }

        private void ButtonImportKeymaze_Click(object sender, EventArgs e)
        {
            m_eImport = EImportMode.eKM700;
            buttonKeymazeImport.Enabled = false;
            listBoxCOM.Enabled = false;
            checkedListBoxDates.Enabled = false;
            textBoxCOMSpeed.Enabled = false;
            buttonSerial.Enabled = false;

            string strCom = (string)listBoxCOM.SelectedItem;
            if (strCom == null) return;
            if (textBoxCOMSpeed.Text == "") return;

            m_oKeymaze.Open(strCom, int.Parse(textBoxCOMSpeed.Text));

            m_lstIndices = new int[checkedListBoxDates.CheckedIndices.Count];
            for (int iActivity = 0; iActivity < checkedListBoxDates.CheckedIndices.Count; iActivity++)
                m_lstIndices[iActivity] = checkedListBoxDates.CheckedIndices[iActivity];

            m_oThreadImport = new Thread(new ThreadStart(WorkThreadKeymaze))
            {
                Name = "ThreadDecoding"
            };
            m_oThreadImport.Start();

            timerThread.Start();
        }

        private void TimerThread_Tick(object sender, EventArgs e)
        {
            if (m_oThreadImport.ThreadState == System.Threading.ThreadState.Stopped)
            {
                timerThread.Stop();

                if (m_eImport == EImportMode.eRS200)
                {
                    progressBarRS200.Value = 0;
                    progressBarLevel.Value = 0;
                    labelSatus.Text = "Décodage terminé";
                    buttonRS200Import.Enabled = true;
                }
                else
                {
                    progressBarKeymaze.Value = 100; 
                    m_lstIndices = null;
                    m_oKeymaze.Close();

                    buttonKeymazeImport.Enabled = true;
                    listBoxCOM.Enabled = true;
                    checkedListBoxDates.Enabled = true;
                    textBoxCOMSpeed.Enabled = true;
                    buttonSerial.Enabled = true;
                    labelProgress.Text = "";
                    progressBarKeymaze.Value = 0;
                }

                m_oRdm.LoadRunnerData(m_strDataDirectory);
                UpdateList();
				UpdateGraphSummary();
                string strDate = "";
                string strFile = m_strDataFile.Replace('/', '\\');
                int nData = m_oRdm.GetRunnerDataCount();
				for (int iData = 0; iData < nData; iData++)
				{
                    RunnerData data = m_oRdm.GetRunnerData(iData);
					if (data.m_strFileName == strFile)
					{
						strDate = data.m_dDate.ToString("dd-MM-yy HH:mm:ss");
						break;
					}
				}

				int iSel = comboBoxDate.FindString(strDate);
				comboBoxDate.SelectedIndex = iSel;
			}
            else
            {
                if (m_eImport == EImportMode.eRS200)
                {
                    progressBarRS200.Value = (int)m_oRS200.GetProgressValue();
                    progressBarLevel.Value = (int)m_oRS200.GetLevelValue();
                    labelSatus.Text = m_oRS200.GetStatus();
                }
                else
                {
                    progressBarKeymaze.Value = (int)m_oKeymaze.GetProgressValue();
                    ActivityKeymaze a = (ActivityKeymaze)m_lstActivities[m_lstIndices[m_iIndice]];
                    labelProgress.Text = a.m_dStart.ToString("dd-MM-yy");
                }
            }
        }


        private void UpdateList()
        {
            listViewData.Items.Clear();
			comboBoxDate.Items.Clear();

            int nData = m_oRdm.GetRunnerDataCount();
            for (int iData = 0; iData < nData; iData++)
            {
                RunnerData data = m_oRdm.GetRunnerData(iData);
				ListViewItem item = listViewData.Items.Add(data.m_dDate.ToString("dd-MM-yy HH:mm:ss"));
                item.SubItems.Add(data.m_dDistance.ToString("F3"));
                item.SubItems.Add(data.m_dTime.ToString("HH:mm:ss"));
                item.SubItems.Add(data.m_dSpeed.ToString("F3"));
                item.SubItems.Add(data.m_dAvgHR.ToString("F0"));
                item.SubItems.Add(data.m_dMaxHR.ToString("F0"));
                item.SubItems.Add(data.m_dCalories.ToString("F0"));
				
				comboBoxDate.Items.Add(data.m_dDate.ToString("dd-MM-yy HH:mm:ss"));
			}
		}

        private void UpdateGraphSummary()
        {
            GraphPane myPane = zedGraphControlSummary.GraphPane;
			myPane.CurveList.Clear();

            string strTitle = (string) comboBoxGraphType.SelectedItem;
            if (strTitle == null || strTitle == "") return;
            
            double x, y;
            PointPairList lstPts = new PointPairList();
            int nData = m_oRdm.GetRunnerDataCount();
            for (int iData = 0; iData < nData; iData++)
            {
                RunnerData data = m_oRdm.GetRunnerData(iData);

				x = data.m_dDate.ToOADate();
				y = 0.0;
				if (strTitle == "Distance") y = data.m_dDistance;
				else if (strTitle == "Temps") y = data.m_dTime.ToOADate();
				else if (strTitle == "Vitesse") y = data.m_dSpeed;
				else if (strTitle == "FC Moyen") y = data.m_dAvgHR;
				else if (strTitle == "FC Max") y = data.m_dMaxHR;
				else if (strTitle == "Calories") y = data.m_dCalories;
                
                lstPts.Add(x, y);
            }

            LineItem myCurve = myPane.AddCurve(strTitle, lstPts, Color.Red, SymbolType.Diamond);

			myPane.Title.IsVisible = false;
			myPane.Legend.IsVisible = false;
	
			myPane.XAxis.Title.Text = "Date";
			myPane.XAxis.MajorGrid.IsVisible = true;
			myPane.XAxis.MajorGrid.Color = Color.LightGray;
			myPane.XAxis.Type = AxisType.Date;
			myPane.XAxis.Scale.Format = "dd-MM-yy";

			myPane.YAxis.Title.Text = strTitle;
			myPane.YAxis.MajorGrid.IsVisible = true;
			myPane.YAxis.MajorGrid.Color = Color.LightGray;
			if (strTitle == "Temps")
			{
				myPane.YAxis.Type = AxisType.Date;
				myPane.YAxis.Scale.Format = "HH:mm:ss";
			}
			else
			{
				myPane.YAxis.Type = AxisType.Linear;
				myPane.YAxis.Scale.Format = "";
			}

			myCurve.Line.Width = 2.0F;
			myCurve.Line.Fill = new Fill(Color.White, Color.Red, 45F);

			myCurve.Symbol.Size = 8.0F;
			myCurve.Symbol.Fill = new Fill(Color.White);

			myPane.Chart.Fill = new Fill(Color.White, Color.FromArgb(255, 255, 210), -45F);

			zedGraphControlSummary.AxisChange();
			zedGraphControlSummary.Refresh();
        }

        private void UpdateGraphDetails()
        {
            GraphPane myPane = zedGraphControlDetails.GraphPane;
            myPane.CurveList.Clear();

            string strTitle = (string)comboBoxMode.SelectedItem;
            if (strTitle == null || strTitle == "") return;

            double x, y;
            PointPairList lstPts = new PointPairList();
            int nMark = m_oRdm.GetMarkCount();
            for (int iMark = 0; iMark < nMark; iMark++)
            {
                Mark oMark = m_oRdm.GetMark(iMark);
                if (oMark == null) continue;

                x = oMark.m_dTime.ToOADate();
                y = 0.0;
                if (strTitle == "Fréquence cardiaque") y = oMark.m_dHeartRate;
                else if (strTitle == "Vitesse") y = oMark.m_dSpeed;
                else if (strTitle == "Altitude") y = oMark.m_dElevation;
                
                lstPts.Add(x, y);
            }

            LineItem myCurve = myPane.AddCurve(strTitle, lstPts, Color.Red, SymbolType.Diamond);

            myPane.Title.IsVisible = false;
            myPane.Legend.IsVisible = false;

            myPane.XAxis.Title.Text = "Heure";
            myPane.XAxis.MajorGrid.IsVisible = true;
            myPane.XAxis.MajorGrid.Color = Color.LightGray;
            myPane.XAxis.Type = AxisType.Date;
            myPane.XAxis.Scale.Format = "HH:mm:ss";

            myPane.YAxis.Title.Text = strTitle;
            myPane.YAxis.MajorGrid.IsVisible = true;
            myPane.YAxis.MajorGrid.Color = Color.LightGray;
            myPane.YAxis.Type = AxisType.Linear;
            myPane.YAxis.Scale.Format = "";

            myCurve.Line.Width = 2.0F;
            myCurve.Line.Fill = new Fill(Color.White, Color.Red, 45F);

            myCurve.Symbol.Size = 8.0F;
            myCurve.Symbol.Fill = new Fill(Color.White);

            myPane.Chart.Fill = new Fill(Color.White, Color.FromArgb(255, 255, 210), -45F);

            zedGraphControlDetails.AxisChange();
            zedGraphControlDetails.Refresh();
        }
        
        private void UpdateMap()
		{
            NumberFormatInfo nfi = new NumberFormatInfo
            {
                NumberDecimalSeparator = "."
            };

            String strPolyline = "";

            int nMarks = m_oRdm.GetMarkCount();

            for (int iPt = 0; iPt < nMarks; iPt++)
			{
                Mark oMark = m_oRdm.GetMark(iPt);
				if (oMark == null) continue;
				string strLongi = oMark.m_dLongi.ToString(nfi);
				string strLati = oMark.m_dLati.ToString(nfi);
				strPolyline += strLongi + "," + strLati;
                if (iPt != nMarks - 1) strPolyline += ",";
			}

			object[] oItinerary = new object[1];
			oItinerary[0] = strPolyline;

			webBrowserMap.Document.InvokeScript("displayItinerary", oItinerary);
		}

		private void FormRunner_Load(object sender, EventArgs e)
        {
            string strValue = m_oIniFileAccess.ReadValue("PARAMETERS", "MODE");
            if (strValue == "2")
            {
                tabControlRunner.TabPages.Remove(tabPageImportRS200);
                tabControlRunner.TabPages.Remove(tabPageImportKeymaze500);
                checkBoxModify.Visible = false;
                buttonRedo.Visible = false;
                buttonUndo.Visible = false;
            }
            else if (strValue == "1")
            {
                tabControlRunner.TabPages.Remove(tabPageImportRS200);
                checkBoxModify.Visible = false;
                buttonRedo.Visible = false;
                buttonUndo.Visible = false;
            }
            else
            {
                tabControlRunner.TabPages.Remove(tabPageImportKeymaze500);
                splitContainer.Panel1Collapsed = true;
                comboBoxMode.Visible = false;
            }
            
            buttonRedo.Enabled = false;
            buttonUndo.Enabled = false;
            
            m_oRdm.LoadRunnerData(m_strDataDirectory);
            UpdateList();

			comboBoxGraphType.SelectedIndex = 0;

            string[] lstPorts = m_oKeymaze.GetPortNames();
            for (int iPort = 0; iPort < lstPorts.Length; iPort++)
            {
                listBoxCOM.Items.Add(lstPorts[iPort]);
            }
            if (lstPorts.Length > 0) listBoxCOM.SelectedIndex = 0;

            listBoxCOM.Text = m_oIniFileAccess.ReadValue("KEYMAZE", "SerialPort");
            textBoxCOMSpeed.Text = m_oIniFileAccess.ReadValue("KEYMAZE", "SerialSpeed");
		}

		private void ComboBoxGraphType_SelectedIndexChanged(object sender, EventArgs e)
		{
			UpdateGraphSummary();
		}

		private void FormRunner_FormClosing(object sender, FormClosingEventArgs e)
		{
            if (checkBoxModify.Checked == true) m_oRdm.SaveMarks();
            m_oIniFileAccess.WriteValue("KEYMAZE", "SerialPort", listBoxCOM.Text);
            m_oIniFileAccess.WriteValue("KEYMAZE", "SerialSpeed", textBoxCOMSpeed.Text);
        }

        private void ComboBoxDate_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (checkBoxModify.Checked == true)
            {
                checkBoxModify.Checked = false;
                buttonUndo.Enabled = true;
                buttonRedo.Enabled = true;
                m_oRdm.SaveMarks();
            }

            object[] oCoordinates = m_oRdm.LoadMarks(comboBoxDate.SelectedIndex);
            if (oCoordinates != null) webBrowserMap.Document.InvokeScript("centerMapOn", oCoordinates);

            UpdateMap();
            
            comboBoxMode.SelectedIndex = 0;
        }

        private void ComboBoxMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateGraphDetails();
        }

        private void CheckBoxModify_Click(object sender, EventArgs e)
        {
            buttonUndo.Enabled = checkBoxModify.Checked;
            buttonRedo.Enabled = checkBoxModify.Checked;
            comboBoxDate.Enabled = !checkBoxModify.Checked;
            if (!checkBoxModify.Checked) m_oRdm.SaveMarks();
        }

        private void ButtonUndo_Click(object sender, EventArgs e)
        {
            if (checkBoxModify.Checked == true)
            {
                m_oRdm.UndoMark();
                UpdateMap();
            }
        }

        private void ButtonRedo_Click(object sender, EventArgs e)
        {
            if (checkBoxModify.Checked == true)
            {
                m_oRdm.RedoMark();
                UpdateMap();
            }
        }

        private void ButtonOpenKeymaze_Click(object sender, EventArgs e)
        {
            string strCom = (string) listBoxCOM.SelectedItem;
            if (strCom == null) return;
            if (textBoxCOMSpeed.Text == "") return;

            m_oKeymaze.SetDirectory(m_strDataDirectory);
            m_oKeymaze.Open(strCom, int.Parse(textBoxCOMSpeed.Text));
            m_oKeymaze.GetInformation();
            m_lstActivities = m_oKeymaze.GetActivities();
            m_oKeymaze.Close();

            if (m_lstActivities != null)
            {
                int nActivities = m_lstActivities.Count;
                for (int iActivity = 0; iActivity < nActivities; iActivity++)
                {
                    ActivityKeymaze oActivity = (ActivityKeymaze)m_lstActivities[iActivity];
                    checkedListBoxDates.Items.Add(oActivity.m_dStart.ToShortDateString());
                }
            }
        }

        private void ZedGraphControlDetails_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Middle) return;

            GraphPane myPane = zedGraphControlDetails.GraphPane;
            PointF mousePt = new PointF(e.X, e.Y);
            myPane.FindNearestPoint(mousePt, out _, out int iPt);
            if (iPt >= 0 && iPt < m_oRdm.GetMarkCount())
            {
                Mark oMark = m_oRdm.GetMark(iPt);
                if (oMark != null)
                {
                    object[] oParam = new object[3];
                    oParam[0] = oMark.m_dLongi;
                    oParam[1] = oMark.m_dLati;
                    oParam[2] = 17;
                    webBrowserMap.Document.InvokeScript("centerZoom", oParam);
                }
            }
        }
	}
}