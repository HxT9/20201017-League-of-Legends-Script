using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace External_Spell_Tracker
{
    public class DDragonApi
    {
        const string API_Version = "https://ddragon.leagueoflegends.com/api/versions.json";
        const string API_ChampionInfo = "https://ddragon.leagueoflegends.com/cdn/{Version}/data/en_US/champion/{ChampionName}.json";
        const string API_ChampionIcon = "https://ddragon.leagueoflegends.com/cdn/{Version}/img/champion/{ChampionName}.png";
        const string API_ChampionPassive = "https://ddragon.leagueoflegends.com/cdn/{Version}/img/passive/{SpellName}";
        const string API_ChampionSpell = "https://ddragon.leagueoflegends.com/cdn/{Version}/img/spell/{SpellName}.png";
        const string API_LH_Data = "https://127.0.0.1:2999/liveclientdata/playerlist";

        string version;
        WebClient webClient;

        public DDragonApi()
        {
            webClient = new WebClient();
            ServicePointManager.ServerCertificateValidationCallback += (sender, cert, chain, sslPolicyErrors) => true;

            string response = webClient.DownloadString(API_Version);
            dynamic versions = JsonConvert.DeserializeObject(response);
            version = versions.First;
        }

        public Image getImage(string url)
        {
            try
            {
                return Image.FromStream(webClient.OpenRead(url));
            }
            catch (Exception e)
            {
                MessageBox.Show("Can't get image: " + url);
                return null;
            }
        }

        public Image getChampionIcon(string championName)
        {
            return getImage(API_ChampionIcon.Replace("{ChampionName}", championName).Replace("{Version}", version));
        }

        public Image getChampionPassive(string championName)
        {
            string response, spellName;

            response = webClient.DownloadString(API_ChampionInfo.Replace("{ChampionName}", championName).Replace("{Version}", version));
            JObject championInfo = JObject.Parse(response);
            spellName = (string)championInfo["data"].First.First["passive"]["image"]["full"];

            return getImage(API_ChampionPassive.Replace("{SpellName}", spellName).Replace("{Version}", version));
        }

        public Image getChampionSpell(string championName, Spells spell)
        {
            string response, spellName;
            switch (spell)
            {
                case Spells.SpellQ:
                case Spells.SpellW:
                case Spells.SpellE:
                case Spells.SpellR:
                    response = webClient.DownloadString(API_ChampionInfo.Replace("{ChampionName}", championName).Replace("{Version}", version));
                    JObject championInfo = JObject.Parse(response);
                    spellName = (string)championInfo["data"].First.First["spells"][(int)spell]["id"];
                    break;

                case Spells.SpellS1:
                case Spells.SpellS2:
                    response = webClient.DownloadString(API_LH_Data);
                    List<Player> players = JsonConvert.DeserializeObject<List<Player>>(response);
                    Player t = null;
                    spellName = "";

                    t = players
                        .Where(p => p.championName
                                        .Replace("Wukong", "MonkeyKing") == championName).FirstOrDefault();

                    if (t != null)
                    {
                        if (spell == Spells.SpellS1)
                            spellName = t.summonerSpells.summonerSpellOne.displayName;
                        else
                            spellName = t.summonerSpells.summonerSpellTwo.displayName;
                    }

                    switch (spellName)
                    {
                        case "Heal":
                            spellName = "SummonerHeal";
                            break;
                        case "Ghost":
                            spellName = "SummonerHaste";
                            break;
                        case "Barrier":
                            spellName = "SummonerBarrier";
                            break;
                        case "Exhaust":
                            spellName = "SummonerExhaust";
                            break;
                        case "Mark":
                            spellName = "SummonerSnowball";
                            break;
                        case "Clarity":
                            spellName = "SummonerMana";
                            break;
                        case "Flash":
                            spellName = "SummonerFlash";
                            break;
                        case "Teleport":
                            spellName = "SummonerTeleport";
                            break;
                        case "Smite":
                            spellName = "SummonerSmite";
                            break;
                        case "Cleanse":
                            spellName = "SummonerBoost";
                            break;
                        case "Ignite":
                            spellName = "SummonerDot";
                            break;
                    }
                    break;

                default:
                    return null;
            }

            return getImage(API_ChampionSpell.Replace("{Version}", version).Replace("{SpellName}", spellName));
        }

        public class Player
        {
            public string championName { get; set; }
            public bool isBot { get; set; }
            public bool isDead { get; set; }
            public List<Item> items { get; set; }
            public int level { get; set; }
            public string position { get; set; }
            public string rawChampionName { get; set; }
            public double respawnTimer { get; set; }
            public Runes runes { get; set; }
            public Scores scores { get; set; }
            public int skinID { get; set; }
            public string summonerName { get; set; }
            public SummonerSpells summonerSpells { get; set; }
            public string team { get; set; }

            public class Item
            {
                public bool canUse { get; set; }
                public bool consumable { get; set; }
                public int count { get; set; }
                public string displayName { get; set; }
                public int itemID { get; set; }
                public int price { get; set; }
                public string rawDescription { get; set; }
                public string rawDisplayName { get; set; }
                public int slot { get; set; }
            }
            public class SummonerSpells
            {
                public SummonerSpellOne summonerSpellOne { get; set; }
                public SummonerSpellTwo summonerSpellTwo { get; set; }
                public class SummonerSpellOne
                {
                    public string displayName { get; set; }
                    public string rawDescription { get; set; }
                    public string rawDisplayName { get; set; }
                }
                public class SummonerSpellTwo
                {
                    public string displayName { get; set; }
                    public string rawDescription { get; set; }
                    public string rawDisplayName { get; set; }
                }
            }
            public class Runes
            {
                public Keystone keystone { get; set; }
                public PrimaryRuneTree primaryRuneTree { get; set; }
                public SecondaryRuneTree secondaryRuneTree { get; set; }
                public class Keystone
                {
                    public string displayName { get; set; }
                    public int id { get; set; }
                    public string rawDescription { get; set; }
                    public string rawDisplayName { get; set; }
                }
                public class PrimaryRuneTree
                {
                    public string displayName { get; set; }
                    public int id { get; set; }
                    public string rawDescription { get; set; }
                    public string rawDisplayName { get; set; }
                }
                public class SecondaryRuneTree
                {
                    public string displayName { get; set; }
                    public int id { get; set; }
                    public string rawDescription { get; set; }
                    public string rawDisplayName { get; set; }
                }
            }

            public class Scores
            {
                public int assists { get; set; }
                public int creepScore { get; set; }
                public int deaths { get; set; }
                public int kills { get; set; }
                public double wardScore { get; set; }
            }
        }
    }
}
