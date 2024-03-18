using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace External_Spell_Tracker
{
    enum Offset
    {
        GameTime = 0x3136040,
        HeroList = 0x18a0014,
        LocalPlayer = 0x313d26c,
        Team = 0x34,
        SpellBook = 0x24C8,
        ChampionName = 0x2f48,
        SpellSlots = 0x488,
        SpellSlotSpellLvl = 0x1C,
        SpellSlotSpellready = 0x24,
    }

    public enum Spells
    {
        SpellQ = 0,
        SpellW,
        SpellE,
        SpellR,
        SpellS1,
        SpellS2
    }

    class SpellTracker
    {
        const int PROCESS_WM_READ = 0x0010;

        [DllImport("kernel32.dll")]
        public static extern System.UInt32 OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll")]
        public static extern bool ReadProcessMemory(uint hProcess, uint lpBaseAddress, byte[] lpBuffer, uint dwSize, ref int lpNumberOfBytesRead);

        const string LolProcessName = "League of Legends";

        private uint processHandle;
        private uint baseAddress;
        public List<Champion> champions;

        public SpellTracker()
        {
            if (Process.GetProcessesByName(LolProcessName).Length == 0)
            {
                return;
            }

            Process LolProcess = Process.GetProcessesByName(LolProcessName)[0];

            baseAddress = (uint)LolProcess.MainModule.BaseAddress;

            processHandle = OpenProcess(PROCESS_WM_READ, false, LolProcess.Id);

            if (processHandle != 0)
            {
                champions = new List<Champion>();

                uint hero;
                uint temp;

                temp = (uint)ReadInt(baseAddress + (uint)Offset.HeroList);
                temp = (uint)ReadInt(temp + 4);

                Champion me = new Champion(this, (uint)ReadInt(baseAddress + (uint)Offset.LocalPlayer));

                while((hero = (uint)ReadInt(temp)) > 0)
                {
                    if (champions.Where(c => c.Address == hero).Count() > 0) break;

                    Champion cTemp = new Champion(this, hero);

                    if (cTemp.Team != me.Team)
                        champions.Add(new Champion(this, hero));

                    temp += 4;
                }
            }
        }

        public System.Int32 ReadInt(uint address)
        {
            System.Int32 ret = 0;
            byte[] data = new byte[4];
            int temp = 0;

            ReadProcessMemory(processHandle, address, data, 4, ref temp);
            ret = BitConverter.ToInt32(data, 0);
            return ret;
        }

        public System.Single ReadFloat(uint address)
        {
            System.Single ret = 0;
            byte[] data = new byte[4];
            int temp = 0;

            ReadProcessMemory(processHandle, address, data, 4, ref temp);
            ret = BitConverter.ToSingle(data, 0);
            return ret;
        }

        public System.String ReadString(uint address, int size)
        {
            System.String ret = "";
            byte[] data = new byte[size];
            int temp = 0;

            ReadProcessMemory(processHandle, address, data, (uint)size, ref temp);

            ret = Encoding.ASCII.GetString(data);
            if (ret.IndexOf((char)0) >= 0)
                ret = ret.Substring(0, ret.IndexOf((char)0));
            return ret;
        }

        public void Refresh()
        {
            float gameTime = ReadFloat(baseAddress + (uint)Offset.GameTime);
            foreach (Champion c in champions)
            {
                for(int i = 0; i < 6; i++)
                {
                    uint spellSlotAddress = (uint)ReadInt(c.Address + (uint)Offset.SpellBook + (uint)Offset.SpellSlots + (uint)(i * 4));
                    if (spellSlotAddress > 0) {
                        float spellReady = ReadFloat(spellSlotAddress + (uint)Offset.SpellSlotSpellready);
                        spellReady -= gameTime;
                        if (spellReady < 0) spellReady = 0;

                        Control[] cd_labels= c.heroEntry.Controls.Find("CD_" + i, true);
                        if (cd_labels.Length > 0)
                        {
                            ((Label)cd_labels[0]).Invoke((MethodInvoker)(() => ((Label)cd_labels[0]).Text = ((int)spellReady).ToString()));
                        }
                    }
                }
            }
        }

        public class Champion
        {
            SpellTracker st;
            public HeroEntry heroEntry { get; set; }
            public Champion(SpellTracker spellTracker, uint address)
            {
                this.address = address;
                st = spellTracker;

                name = st.ReadString(address + (uint)Offset.ChampionName, 15);
                Team = st.ReadInt(address + (uint)Offset.Team);
            }

            private uint address;

            public uint Address
            {
                get { return address; }
            }

            private string name;

            public string Name
            {
                get { return name; }
                set { name = value; }
            }

            public int Team { get; private set; }

            public float getSpellReady(Spells spell)
            {
                uint spellSlot = (uint)st.ReadInt(address + (uint)Offset.SpellBook + (uint)Offset.SpellSlots + (uint)((int)spell * 4));
                return st.ReadFloat(spellSlot + (uint)Offset.SpellSlotSpellready);
            }
            
            public int getSpellLvl(Spells spell)
            {
                uint spellSlot = (uint)st.ReadInt(address + (uint)Offset.SpellBook + (uint)Offset.SpellSlots + (uint)((int)spell * 4));
                return st.ReadInt(spellSlot + (uint)Offset.SpellSlotSpellLvl);
            }
        }
    }
}
