namespace Runner
{
    partial class FormRunner
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormRunner));
            this.tabControlRunner = new System.Windows.Forms.TabControl();
            this.tabPageDonnées = new System.Windows.Forms.TabPage();
            this.listViewData = new System.Windows.Forms.ListView();
            this.columnHeaderDate = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderDistance = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderTime = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderSpeed = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderAvgHR = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderMaxHR = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderCalories = new System.Windows.Forms.ColumnHeader();
            this.tabPageSummary = new System.Windows.Forms.TabPage();
            this.comboBoxGraphType = new System.Windows.Forms.ComboBox();
            this.zedGraphControlSummary = new ZedGraph.ZedGraphControl();
            this.tabPageDetails = new System.Windows.Forms.TabPage();
            this.buttonSearch = new System.Windows.Forms.Button();
            this.checkBoxModify = new System.Windows.Forms.CheckBox();
            this.buttonRedo = new System.Windows.Forms.Button();
            this.buttonUndo = new System.Windows.Forms.Button();
            this.textBoxSearch = new System.Windows.Forms.TextBox();
            this.comboBoxMode = new System.Windows.Forms.ComboBox();
            this.comboBoxDate = new System.Windows.Forms.ComboBox();
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.zedGraphControlDetails = new ZedGraph.ZedGraphControl();
            this.webBrowserMap = new System.Windows.Forms.WebBrowser();
            this.tabPageImportRS200 = new System.Windows.Forms.TabPage();
            this.labelLevel = new System.Windows.Forms.Label();
            this.labelDecoding = new System.Windows.Forms.Label();
            this.buttonRS200Import = new System.Windows.Forms.Button();
            this.labelSatus = new System.Windows.Forms.Label();
            this.progressBarLevel = new System.Windows.Forms.ProgressBar();
            this.progressBarRS200 = new System.Windows.Forms.ProgressBar();
            this.tabPageImportKeymaze500 = new System.Windows.Forms.TabPage();
            this.labelFile = new System.Windows.Forms.Label();
            this.labelProgress = new System.Windows.Forms.Label();
            this.progressBarKeymaze = new System.Windows.Forms.ProgressBar();
            this.checkedListBoxDates = new System.Windows.Forms.CheckedListBox();
            this.buttonKeymazeImport = new System.Windows.Forms.Button();
            this.textBoxCOMSpeed = new System.Windows.Forms.TextBox();
            this.listBoxCOM = new System.Windows.Forms.ListBox();
            this.buttonSerial = new System.Windows.Forms.Button();
            this.timerThread = new System.Windows.Forms.Timer(this.components);
            this.tabControlRunner.SuspendLayout();
            this.tabPageDonnées.SuspendLayout();
            this.tabPageSummary.SuspendLayout();
            this.tabPageDetails.SuspendLayout();
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.tabPageImportRS200.SuspendLayout();
            this.tabPageImportKeymaze500.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControlRunner
            // 
            this.tabControlRunner.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControlRunner.Controls.Add(this.tabPageDonnées);
            this.tabControlRunner.Controls.Add(this.tabPageSummary);
            this.tabControlRunner.Controls.Add(this.tabPageDetails);
            this.tabControlRunner.Controls.Add(this.tabPageImportRS200);
            this.tabControlRunner.Controls.Add(this.tabPageImportKeymaze500);
            this.tabControlRunner.Location = new System.Drawing.Point(12, 12);
            this.tabControlRunner.Name = "tabControlRunner";
            this.tabControlRunner.SelectedIndex = 0;
            this.tabControlRunner.Size = new System.Drawing.Size(611, 463);
            this.tabControlRunner.TabIndex = 4;
            // 
            // tabPageDonnées
            // 
            this.tabPageDonnées.Controls.Add(this.listViewData);
            this.tabPageDonnées.Location = new System.Drawing.Point(4, 22);
            this.tabPageDonnées.Name = "tabPageDonnées";
            this.tabPageDonnées.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageDonnées.Size = new System.Drawing.Size(603, 437);
            this.tabPageDonnées.TabIndex = 1;
            this.tabPageDonnées.Text = "Données";
            this.tabPageDonnées.UseVisualStyleBackColor = true;
            // 
            // listViewData
            // 
            this.listViewData.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listViewData.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderDate,
            this.columnHeaderDistance,
            this.columnHeaderTime,
            this.columnHeaderSpeed,
            this.columnHeaderAvgHR,
            this.columnHeaderMaxHR,
            this.columnHeaderCalories});
            this.listViewData.FullRowSelect = true;
            this.listViewData.GridLines = true;
            this.listViewData.Location = new System.Drawing.Point(6, 6);
            this.listViewData.MultiSelect = false;
            this.listViewData.Name = "listViewData";
            this.listViewData.Size = new System.Drawing.Size(591, 425);
            this.listViewData.TabIndex = 0;
            this.listViewData.UseCompatibleStateImageBehavior = false;
            this.listViewData.View = System.Windows.Forms.View.Details;
            // 
            // columnHeaderDate
            // 
            this.columnHeaderDate.Text = "Date";
            this.columnHeaderDate.Width = 124;
            // 
            // columnHeaderDistance
            // 
            this.columnHeaderDistance.Text = "Distance";
            this.columnHeaderDistance.Width = 76;
            // 
            // columnHeaderTime
            // 
            this.columnHeaderTime.Text = "Temps";
            this.columnHeaderTime.Width = 81;
            // 
            // columnHeaderSpeed
            // 
            this.columnHeaderSpeed.Text = "Vitesse";
            this.columnHeaderSpeed.Width = 65;
            // 
            // columnHeaderAvgHR
            // 
            this.columnHeaderAvgHR.Text = "FC moyenne";
            this.columnHeaderAvgHR.Width = 75;
            // 
            // columnHeaderMaxHR
            // 
            this.columnHeaderMaxHR.Text = "FC Max";
            this.columnHeaderMaxHR.Width = 76;
            // 
            // columnHeaderCalories
            // 
            this.columnHeaderCalories.Text = "Calories";
            this.columnHeaderCalories.Width = 69;
            // 
            // tabPageSummary
            // 
            this.tabPageSummary.Controls.Add(this.comboBoxGraphType);
            this.tabPageSummary.Controls.Add(this.zedGraphControlSummary);
            this.tabPageSummary.Location = new System.Drawing.Point(4, 22);
            this.tabPageSummary.Name = "tabPageSummary";
            this.tabPageSummary.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageSummary.Size = new System.Drawing.Size(603, 437);
            this.tabPageSummary.TabIndex = 2;
            this.tabPageSummary.Text = "Résumé";
            this.tabPageSummary.UseVisualStyleBackColor = true;
            // 
            // comboBoxGraphType
            // 
            this.comboBoxGraphType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxGraphType.FormattingEnabled = true;
            this.comboBoxGraphType.Items.AddRange(new object[] {
            "Distance",
            "Temps",
            "Vitesse",
            "FC Moyen",
            "FC Max",
            "Calories"});
            this.comboBoxGraphType.Location = new System.Drawing.Point(6, 6);
            this.comboBoxGraphType.Name = "comboBoxGraphType";
            this.comboBoxGraphType.Size = new System.Drawing.Size(162, 21);
            this.comboBoxGraphType.TabIndex = 1;
            this.comboBoxGraphType.SelectedIndexChanged += new System.EventHandler(this.comboBoxGraphType_SelectedIndexChanged);
            // 
            // zedGraphControlSummary
            // 
            this.zedGraphControlSummary.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.zedGraphControlSummary.IsShowPointValues = true;
            this.zedGraphControlSummary.Location = new System.Drawing.Point(6, 33);
            this.zedGraphControlSummary.Name = "zedGraphControlSummary";
            this.zedGraphControlSummary.ScrollGrace = 0;
            this.zedGraphControlSummary.ScrollMaxX = 0;
            this.zedGraphControlSummary.ScrollMaxY = 0;
            this.zedGraphControlSummary.ScrollMaxY2 = 0;
            this.zedGraphControlSummary.ScrollMinX = 0;
            this.zedGraphControlSummary.ScrollMinY = 0;
            this.zedGraphControlSummary.ScrollMinY2 = 0;
            this.zedGraphControlSummary.Size = new System.Drawing.Size(591, 398);
            this.zedGraphControlSummary.TabIndex = 0;
            // 
            // tabPageDetails
            // 
            this.tabPageDetails.Controls.Add(this.buttonSearch);
            this.tabPageDetails.Controls.Add(this.checkBoxModify);
            this.tabPageDetails.Controls.Add(this.buttonRedo);
            this.tabPageDetails.Controls.Add(this.buttonUndo);
            this.tabPageDetails.Controls.Add(this.textBoxSearch);
            this.tabPageDetails.Controls.Add(this.comboBoxMode);
            this.tabPageDetails.Controls.Add(this.comboBoxDate);
            this.tabPageDetails.Controls.Add(this.splitContainer);
            this.tabPageDetails.Location = new System.Drawing.Point(4, 22);
            this.tabPageDetails.Name = "tabPageDetails";
            this.tabPageDetails.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageDetails.Size = new System.Drawing.Size(603, 437);
            this.tabPageDetails.TabIndex = 3;
            this.tabPageDetails.Text = "Détails";
            this.tabPageDetails.UseVisualStyleBackColor = true;
            // 
            // buttonSearch
            // 
            this.buttonSearch.Image = ((System.Drawing.Image)(resources.GetObject("buttonSearch.Image")));
            this.buttonSearch.Location = new System.Drawing.Point(444, 3);
            this.buttonSearch.Name = "buttonSearch";
            this.buttonSearch.Size = new System.Drawing.Size(25, 25);
            this.buttonSearch.TabIndex = 11;
            this.buttonSearch.UseVisualStyleBackColor = true;
            this.buttonSearch.Click += new System.EventHandler(this.buttonSearch_Click);
            // 
            // checkBoxModify
            // 
            this.checkBoxModify.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBoxModify.Image = ((System.Drawing.Image)(resources.GetObject("checkBoxModify.Image")));
            this.checkBoxModify.Location = new System.Drawing.Point(496, 3);
            this.checkBoxModify.Name = "checkBoxModify";
            this.checkBoxModify.Size = new System.Drawing.Size(25, 25);
            this.checkBoxModify.TabIndex = 10;
            this.checkBoxModify.UseVisualStyleBackColor = true;
            this.checkBoxModify.Click += new System.EventHandler(this.checkBoxModify_Click);
            // 
            // buttonRedo
            // 
            this.buttonRedo.Image = ((System.Drawing.Image)(resources.GetObject("buttonRedo.Image")));
            this.buttonRedo.Location = new System.Drawing.Point(558, 3);
            this.buttonRedo.Name = "buttonRedo";
            this.buttonRedo.Size = new System.Drawing.Size(25, 25);
            this.buttonRedo.TabIndex = 9;
            this.buttonRedo.UseVisualStyleBackColor = true;
            this.buttonRedo.Click += new System.EventHandler(this.buttonRedo_Click);
            // 
            // buttonUndo
            // 
            this.buttonUndo.Image = ((System.Drawing.Image)(resources.GetObject("buttonUndo.Image")));
            this.buttonUndo.Location = new System.Drawing.Point(527, 3);
            this.buttonUndo.Name = "buttonUndo";
            this.buttonUndo.Size = new System.Drawing.Size(25, 25);
            this.buttonUndo.TabIndex = 8;
            this.buttonUndo.UseVisualStyleBackColor = true;
            this.buttonUndo.Click += new System.EventHandler(this.buttonUndo_Click);
            // 
            // textBoxSearch
            // 
            this.textBoxSearch.Location = new System.Drawing.Point(303, 6);
            this.textBoxSearch.Name = "textBoxSearch";
            this.textBoxSearch.Size = new System.Drawing.Size(135, 20);
            this.textBoxSearch.TabIndex = 6;
            this.textBoxSearch.KeyUp += new System.Windows.Forms.KeyEventHandler(this.textBoxSearch_KeyUp);
            // 
            // comboBoxMode
            // 
            this.comboBoxMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxMode.FormattingEnabled = true;
            this.comboBoxMode.Items.AddRange(new object[] {
            "Fréquence cardiaque",
            "Vitesse",
            "Altitude"});
            this.comboBoxMode.Location = new System.Drawing.Point(136, 6);
            this.comboBoxMode.Name = "comboBoxMode";
            this.comboBoxMode.Size = new System.Drawing.Size(142, 21);
            this.comboBoxMode.TabIndex = 5;
            this.comboBoxMode.SelectedIndexChanged += new System.EventHandler(this.comboBoxMode_SelectedIndexChanged);
            // 
            // comboBoxDate
            // 
            this.comboBoxDate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxDate.FormattingEnabled = true;
            this.comboBoxDate.Location = new System.Drawing.Point(9, 6);
            this.comboBoxDate.Name = "comboBoxDate";
            this.comboBoxDate.Size = new System.Drawing.Size(121, 21);
            this.comboBoxDate.TabIndex = 4;
            this.comboBoxDate.SelectedIndexChanged += new System.EventHandler(this.comboBoxDate_SelectedIndexChanged);
            // 
            // splitContainer
            // 
            this.splitContainer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer.Location = new System.Drawing.Point(6, 31);
            this.splitContainer.Name = "splitContainer";
            this.splitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer.Panel1
            // 
            this.splitContainer.Panel1.Controls.Add(this.zedGraphControlDetails);
            this.splitContainer.Panel1MinSize = 0;
            // 
            // splitContainer.Panel2
            // 
            this.splitContainer.Panel2.Controls.Add(this.webBrowserMap);
            this.splitContainer.Size = new System.Drawing.Size(591, 400);
            this.splitContainer.SplitterDistance = 158;
            this.splitContainer.TabIndex = 3;
            // 
            // zedGraphControlDetails
            // 
            this.zedGraphControlDetails.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.zedGraphControlDetails.EditButtons = System.Windows.Forms.MouseButtons.None;
            this.zedGraphControlDetails.IsEnableHPan = false;
            this.zedGraphControlDetails.IsEnableVPan = false;
            this.zedGraphControlDetails.IsEnableVZoom = false;
            this.zedGraphControlDetails.IsShowPointValues = true;
            this.zedGraphControlDetails.LinkButtons = System.Windows.Forms.MouseButtons.None;
            this.zedGraphControlDetails.Location = new System.Drawing.Point(3, 3);
            this.zedGraphControlDetails.Name = "zedGraphControlDetails";
            this.zedGraphControlDetails.PanButtons = System.Windows.Forms.MouseButtons.None;
            this.zedGraphControlDetails.PanButtons2 = System.Windows.Forms.MouseButtons.None;
            this.zedGraphControlDetails.ScrollGrace = 0;
            this.zedGraphControlDetails.ScrollMaxX = 0;
            this.zedGraphControlDetails.ScrollMaxY = 0;
            this.zedGraphControlDetails.ScrollMaxY2 = 0;
            this.zedGraphControlDetails.ScrollMinX = 0;
            this.zedGraphControlDetails.ScrollMinY = 0;
            this.zedGraphControlDetails.ScrollMinY2 = 0;
            this.zedGraphControlDetails.SelectButtons = System.Windows.Forms.MouseButtons.None;
            this.zedGraphControlDetails.Size = new System.Drawing.Size(585, 152);
            this.zedGraphControlDetails.TabIndex = 4;
            this.zedGraphControlDetails.MouseClick += new System.Windows.Forms.MouseEventHandler(this.zedGraphControlDetails_MouseClick);
            // 
            // webBrowserMap
            // 
            this.webBrowserMap.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.webBrowserMap.Location = new System.Drawing.Point(3, 3);
            this.webBrowserMap.MinimumSize = new System.Drawing.Size(20, 20);
            this.webBrowserMap.Name = "webBrowserMap";
            this.webBrowserMap.Size = new System.Drawing.Size(585, 232);
            this.webBrowserMap.TabIndex = 2;
            this.webBrowserMap.Url = new System.Uri("", System.UriKind.Relative);
            // 
            // tabPageImportRS200
            // 
            this.tabPageImportRS200.Controls.Add(this.labelLevel);
            this.tabPageImportRS200.Controls.Add(this.labelDecoding);
            this.tabPageImportRS200.Controls.Add(this.buttonRS200Import);
            this.tabPageImportRS200.Controls.Add(this.labelSatus);
            this.tabPageImportRS200.Controls.Add(this.progressBarLevel);
            this.tabPageImportRS200.Controls.Add(this.progressBarRS200);
            this.tabPageImportRS200.Location = new System.Drawing.Point(4, 22);
            this.tabPageImportRS200.Name = "tabPageImportRS200";
            this.tabPageImportRS200.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageImportRS200.Size = new System.Drawing.Size(603, 437);
            this.tabPageImportRS200.TabIndex = 0;
            this.tabPageImportRS200.Text = "Import";
            this.tabPageImportRS200.UseVisualStyleBackColor = true;
            // 
            // labelLevel
            // 
            this.labelLevel.AutoSize = true;
            this.labelLevel.Location = new System.Drawing.Point(10, 108);
            this.labelLevel.Name = "labelLevel";
            this.labelLevel.Size = new System.Drawing.Size(61, 13);
            this.labelLevel.TabIndex = 9;
            this.labelLevel.Text = "Niveau son";
            // 
            // labelDecoding
            // 
            this.labelDecoding.AutoSize = true;
            this.labelDecoding.Location = new System.Drawing.Point(10, 147);
            this.labelDecoding.Name = "labelDecoding";
            this.labelDecoding.Size = new System.Drawing.Size(57, 13);
            this.labelDecoding.TabIndex = 8;
            this.labelDecoding.Text = "Décodage";
            // 
            // buttonRS200Import
            // 
            this.buttonRS200Import.Location = new System.Drawing.Point(13, 15);
            this.buttonRS200Import.Name = "buttonRS200Import";
            this.buttonRS200Import.Size = new System.Drawing.Size(75, 23);
            this.buttonRS200Import.TabIndex = 7;
            this.buttonRS200Import.Text = "Importer";
            this.buttonRS200Import.UseVisualStyleBackColor = true;
            this.buttonRS200Import.Click += new System.EventHandler(this.buttonImportRS200_Click);
            // 
            // labelSatus
            // 
            this.labelSatus.AutoSize = true;
            this.labelSatus.Location = new System.Drawing.Point(10, 69);
            this.labelSatus.Name = "labelSatus";
            this.labelSatus.Size = new System.Drawing.Size(133, 13);
            this.labelSatus.TabIndex = 6;
            this.labelSatus.Text = "Aucun décodage en cours";
            // 
            // progressBarLevel
            // 
            this.progressBarLevel.Location = new System.Drawing.Point(73, 103);
            this.progressBarLevel.Maximum = 200;
            this.progressBarLevel.Name = "progressBarLevel";
            this.progressBarLevel.Size = new System.Drawing.Size(346, 23);
            this.progressBarLevel.TabIndex = 5;
            // 
            // progressBarRS200
            // 
            this.progressBarRS200.Location = new System.Drawing.Point(73, 142);
            this.progressBarRS200.Name = "progressBarRS200";
            this.progressBarRS200.Size = new System.Drawing.Size(346, 23);
            this.progressBarRS200.TabIndex = 4;
            // 
            // tabPageImportKeymaze500
            // 
            this.tabPageImportKeymaze500.Controls.Add(this.labelFile);
            this.tabPageImportKeymaze500.Controls.Add(this.labelProgress);
            this.tabPageImportKeymaze500.Controls.Add(this.progressBarKeymaze);
            this.tabPageImportKeymaze500.Controls.Add(this.checkedListBoxDates);
            this.tabPageImportKeymaze500.Controls.Add(this.buttonKeymazeImport);
            this.tabPageImportKeymaze500.Controls.Add(this.textBoxCOMSpeed);
            this.tabPageImportKeymaze500.Controls.Add(this.listBoxCOM);
            this.tabPageImportKeymaze500.Controls.Add(this.buttonSerial);
            this.tabPageImportKeymaze500.Location = new System.Drawing.Point(4, 22);
            this.tabPageImportKeymaze500.Name = "tabPageImportKeymaze500";
            this.tabPageImportKeymaze500.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageImportKeymaze500.Size = new System.Drawing.Size(603, 437);
            this.tabPageImportKeymaze500.TabIndex = 4;
            this.tabPageImportKeymaze500.Text = "Import";
            this.tabPageImportKeymaze500.UseVisualStyleBackColor = true;
            // 
            // labelFile
            // 
            this.labelFile.AutoSize = true;
            this.labelFile.Location = new System.Drawing.Point(44, 212);
            this.labelFile.Name = "labelFile";
            this.labelFile.Size = new System.Drawing.Size(0, 13);
            this.labelFile.TabIndex = 22;
            // 
            // labelProgress
            // 
            this.labelProgress.AutoSize = true;
            this.labelProgress.Location = new System.Drawing.Point(60, 238);
            this.labelProgress.Name = "labelProgress";
            this.labelProgress.Size = new System.Drawing.Size(0, 13);
            this.labelProgress.TabIndex = 21;
            // 
            // progressBarKeymaze
            // 
            this.progressBarKeymaze.Location = new System.Drawing.Point(6, 212);
            this.progressBarKeymaze.Name = "progressBarKeymaze";
            this.progressBarKeymaze.Size = new System.Drawing.Size(175, 23);
            this.progressBarKeymaze.TabIndex = 20;
            // 
            // checkedListBoxDates
            // 
            this.checkedListBoxDates.CheckOnClick = true;
            this.checkedListBoxDates.FormattingEnabled = true;
            this.checkedListBoxDates.Location = new System.Drawing.Point(6, 107);
            this.checkedListBoxDates.Name = "checkedListBoxDates";
            this.checkedListBoxDates.Size = new System.Drawing.Size(175, 94);
            this.checkedListBoxDates.TabIndex = 19;
            // 
            // buttonKeymazeImport
            // 
            this.buttonKeymazeImport.Location = new System.Drawing.Point(187, 178);
            this.buttonKeymazeImport.Name = "buttonKeymazeImport";
            this.buttonKeymazeImport.Size = new System.Drawing.Size(75, 23);
            this.buttonKeymazeImport.TabIndex = 18;
            this.buttonKeymazeImport.Text = "Importer";
            this.buttonKeymazeImport.UseVisualStyleBackColor = true;
            this.buttonKeymazeImport.Click += new System.EventHandler(this.buttonImportKeymaze_Click);
            // 
            // textBoxCOMSpeed
            // 
            this.textBoxCOMSpeed.Location = new System.Drawing.Point(187, 52);
            this.textBoxCOMSpeed.Name = "textBoxCOMSpeed";
            this.textBoxCOMSpeed.Size = new System.Drawing.Size(75, 20);
            this.textBoxCOMSpeed.TabIndex = 16;
            this.textBoxCOMSpeed.Text = "57600";
            // 
            // listBoxCOM
            // 
            this.listBoxCOM.FormattingEnabled = true;
            this.listBoxCOM.Location = new System.Drawing.Point(6, 6);
            this.listBoxCOM.Name = "listBoxCOM";
            this.listBoxCOM.Size = new System.Drawing.Size(175, 95);
            this.listBoxCOM.TabIndex = 15;
            // 
            // buttonSerial
            // 
            this.buttonSerial.Location = new System.Drawing.Point(187, 78);
            this.buttonSerial.Name = "buttonSerial";
            this.buttonSerial.Size = new System.Drawing.Size(75, 23);
            this.buttonSerial.TabIndex = 14;
            this.buttonSerial.Text = "Connexion";
            this.buttonSerial.UseVisualStyleBackColor = true;
            this.buttonSerial.Click += new System.EventHandler(this.buttonOpenKeymaze_Click);
            // 
            // timerThread
            // 
            this.timerThread.Interval = 200;
            this.timerThread.Tick += new System.EventHandler(this.timerThread_Tick);
            // 
            // FormRunner
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(635, 487);
            this.Controls.Add(this.tabControlRunner);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormRunner";
            this.Text = "Runner";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.Load += new System.EventHandler(this.FormRunner_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormRunner_FormClosing);
            this.tabControlRunner.ResumeLayout(false);
            this.tabPageDonnées.ResumeLayout(false);
            this.tabPageSummary.ResumeLayout(false);
            this.tabPageDetails.ResumeLayout(false);
            this.tabPageDetails.PerformLayout();
            this.splitContainer.Panel1.ResumeLayout(false);
            this.splitContainer.Panel2.ResumeLayout(false);
            this.splitContainer.ResumeLayout(false);
            this.tabPageImportRS200.ResumeLayout(false);
            this.tabPageImportRS200.PerformLayout();
            this.tabPageImportKeymaze500.ResumeLayout(false);
            this.tabPageImportKeymaze500.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControlRunner;
        private System.Windows.Forms.TabPage tabPageImportRS200;
        private System.Windows.Forms.Button buttonRS200Import;
        private System.Windows.Forms.Label labelSatus;
        private System.Windows.Forms.ProgressBar progressBarLevel;
        private System.Windows.Forms.ProgressBar progressBarRS200;
        private System.Windows.Forms.TabPage tabPageDonnées;
        private System.Windows.Forms.Label labelDecoding;
        private System.Windows.Forms.Label labelLevel;
        private System.Windows.Forms.Timer timerThread;
        private System.Windows.Forms.ListView listViewData;
        private System.Windows.Forms.ColumnHeader columnHeaderDate;
        private System.Windows.Forms.ColumnHeader columnHeaderDistance;
        private System.Windows.Forms.ColumnHeader columnHeaderTime;
        private System.Windows.Forms.ColumnHeader columnHeaderSpeed;
        private System.Windows.Forms.ColumnHeader columnHeaderAvgHR;
        private System.Windows.Forms.ColumnHeader columnHeaderMaxHR;
        private System.Windows.Forms.ColumnHeader columnHeaderCalories;
        private System.Windows.Forms.TabPage tabPageSummary;
		private System.Windows.Forms.TabPage tabPageDetails;
        private ZedGraph.ZedGraphControl zedGraphControlSummary;
        private System.Windows.Forms.ComboBox comboBoxGraphType;
        private System.Windows.Forms.SplitContainer splitContainer;
        private ZedGraph.ZedGraphControl zedGraphControlDetails;
        private System.Windows.Forms.WebBrowser webBrowserMap;
        private System.Windows.Forms.TextBox textBoxSearch;
        private System.Windows.Forms.ComboBox comboBoxMode;
        private System.Windows.Forms.ComboBox comboBoxDate;
        private System.Windows.Forms.Button buttonRedo;
        private System.Windows.Forms.Button buttonUndo;
        private System.Windows.Forms.CheckBox checkBoxModify;
        private System.Windows.Forms.TabPage tabPageImportKeymaze500;
        private System.Windows.Forms.TextBox textBoxCOMSpeed;
        private System.Windows.Forms.ListBox listBoxCOM;
        private System.Windows.Forms.Button buttonSerial;
        private System.Windows.Forms.CheckedListBox checkedListBoxDates;
        private System.Windows.Forms.Button buttonKeymazeImport;
        private System.Windows.Forms.ProgressBar progressBarKeymaze;
        private System.Windows.Forms.Label labelProgress;
        private System.Windows.Forms.Label labelFile;
        private System.Windows.Forms.Button buttonSearch;
    }
}

