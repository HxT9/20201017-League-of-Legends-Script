using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace External_Spell_Tracker
{
    public partial class HeroEntry : UserControl
    {
        public HeroEntry(DDragonApi api, string name)
        {
            InitializeComponent();
            //this.championName.Text = name;
            if (name != "")
            {
                this.championImage.Image = api.getChampionIcon(name);
                //this.championPassive.Image = api.getChampionPassive(name);
                this.ChampionQImage.Image = api.getChampionSpell(name, Spells.SpellQ);
                this.ChampionWImage.Image = api.getChampionSpell(name, Spells.SpellW);
                this.ChampionEImage.Image = api.getChampionSpell(name, Spells.SpellE);
                this.ChampionRImage.Image = api.getChampionSpell(name, Spells.SpellR);
                this.ChampionS1Image.Image = api.getChampionSpell(name, Spells.SpellS1);
                this.ChampionS2Image.Image = api.getChampionSpell(name, Spells.SpellS2);
            }
        }

        private void HeroEntry_Load(object sender, EventArgs e)
        {
        }
    }
}
