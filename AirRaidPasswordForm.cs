using System;
using System.Drawing;
using System.Windows.Forms;

namespace ste.pa.pamanager
{
    internal sealed class AirRaidPasswordForm : Form
    {
        private readonly TextBox passwordTextBox = new TextBox();
        private readonly Label messageLabel = new Label();
        private readonly Button verifyButton = new Button();

        internal AirRaidPasswordForm()
        {
            Font = new Font("Microsoft Sans Serif", 12F, FontStyle.Regular, GraphicsUnit.Point, ((byte)(0)));
            Text = "\u7A7A\u8972\u8B66\u5831\u5BC6\u78BC\u9A57\u8B49";
            ClientSize = new Size(440, 225);
            FormBorderStyle = FormBorderStyle.Fixed3D;
            MaximizeBox = false;
            MinimizeBox = false;
            ShowInTaskbar = false;
            StartPosition = FormStartPosition.CenterParent;

            var instruction = new Label { AutoSize = false, Location = new Point(22, 18), Size = new Size(395, 46), Text = "\u767C\u9001\u7A7A\u8972\u8B66\u5831\u524D\uFF0C\u8ACB\u8F38\u5165\u5BC6\u78BC\u9032\u884C\u9A57\u8B49\u3002" };
            var passwordLabel = new Label { AutoSize = true, Location = new Point(22, 81), Text = "\u5BC6\u78BC\uFF1A" };
            passwordTextBox.Location = new Point(100, 76);
            passwordTextBox.Size = new Size(315, 26);
            passwordTextBox.PasswordChar = '*';
            passwordTextBox.TabIndex = 0;

            messageLabel.AutoSize = false;
            messageLabel.ForeColor = Color.Firebrick;
            messageLabel.Location = new Point(22, 111);
            messageLabel.Size = new Size(393, 32);

            var changeButton = new Button { Location = new Point(22, 165), Size = new Size(115, 40), Text = "\u4FEE\u6539\u5BC6\u78BC", TabIndex = 1 };
            changeButton.Click += ChangeButton_Click;
            verifyButton.Location = new Point(175, 165);
            verifyButton.Size = new Size(115, 40);
            verifyButton.Text = "\u78BA\u8A8D";
            verifyButton.TabIndex = 2;
            verifyButton.Click += VerifyButton_Click;
            var cancelButton = new Button { Location = new Point(300, 165), Size = new Size(115, 40), Text = "\u53D6\u6D88", TabIndex = 3, DialogResult = DialogResult.Cancel };

            AcceptButton = verifyButton;
            CancelButton = cancelButton;
            Controls.AddRange(new Control[] { instruction, passwordLabel, passwordTextBox, messageLabel, changeButton, verifyButton, cancelButton });
        }

        private void VerifyButton_Click(object sender, EventArgs e)
        {
            string failureReason;
            if (!AirRaidPasswordService.TryVerify(passwordTextBox.Text, out failureReason))
            {
                passwordTextBox.Clear();
                passwordTextBox.Focus();
                messageLabel.Text = failureReason;
                return;
            }
            DialogResult = DialogResult.OK;
            Close();
        }

        private void ChangeButton_Click(object sender, EventArgs e)
        {
            using (AirRaidChangePasswordForm dialog = new AirRaidChangePasswordForm())
            {
                dialog.ShowDialog(this);
            }
            passwordTextBox.Clear();
            passwordTextBox.Focus();
        }
    }

    internal sealed class AirRaidChangePasswordForm : Form
    {
        private readonly TextBox currentTextBox = new TextBox();
        private readonly TextBox newTextBox = new TextBox();
        private readonly TextBox confirmTextBox = new TextBox();
        private readonly Label messageLabel = new Label();

        internal AirRaidChangePasswordForm()
        {
            Font = new Font("Microsoft Sans Serif", 12F, FontStyle.Regular, GraphicsUnit.Point, ((byte)(0)));
            Text = "\u4FEE\u6539\u7A7A\u8972\u8B66\u5831\u5BC6\u78BC";
            ClientSize = new Size(470, 300);
            FormBorderStyle = FormBorderStyle.Fixed3D;
            MaximizeBox = false;
            MinimizeBox = false;
            ShowInTaskbar = false;
            StartPosition = FormStartPosition.CenterParent;

            AddPasswordField("\u820A\u5BC6\u78BC\uFF1A", currentTextBox, 25, 27, 145);
            AddPasswordField("\u65B0\u5BC6\u78BC\uFF1A", newTextBox, 25, 72, 145);
            AddPasswordField("\u78BA\u8A8D\u65B0\u5BC6\u78BC\uFF1A", confirmTextBox, 25, 117, 145);
            messageLabel.AutoSize = false;
            messageLabel.ForeColor = Color.Firebrick;
            messageLabel.Location = new Point(25, 165);
            messageLabel.Size = new Size(420, 35);

            var saveButton = new Button { Location = new Point(205, 235), Size = new Size(115, 40), Text = "\u78BA\u8A8D" };
            saveButton.Click += SaveButton_Click;
            var cancelButton = new Button { Location = new Point(330, 235), Size = new Size(115, 40), Text = "\u53D6\u6D88", DialogResult = DialogResult.Cancel };
            AcceptButton = saveButton;
            CancelButton = cancelButton;
            Controls.AddRange(new Control[] { messageLabel, saveButton, cancelButton });
        }

        private void AddPasswordField(string labelText, TextBox textBox, int left, int top, int textBoxLeft)
        {
            Controls.Add(new Label { AutoSize = true, Location = new Point(left, top + 5), Text = labelText });
            textBox.Location = new Point(textBoxLeft, top);
            textBox.Size = new Size(300, 26);
            textBox.PasswordChar = '*';
            Controls.Add(textBox);
        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            string failureReason;
            if (!AirRaidPasswordService.TryChangePassword(currentTextBox.Text, newTextBox.Text, confirmTextBox.Text, out failureReason))
            {
                currentTextBox.Clear();
                newTextBox.Clear();
                confirmTextBox.Clear();
                currentTextBox.Focus();
                messageLabel.Text = failureReason;
                return;
            }
            MessageBox.Show("\u5BC6\u78BC\u5DF2\u4FEE\u6539\u3002", Text, MessageBoxButtons.OK, MessageBoxIcon.Information);
            DialogResult = DialogResult.OK;
            Close();
        }
    }
}
