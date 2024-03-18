using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using ICSharpCode.SharpZipLib.GZip;
using ICSharpCode.SharpZipLib.Tar;
using Newtonsoft.Json;

namespace Dragontail_Downloader
{
    class Program
    {
        static string downloadPath = @"E:\alber\source\repos\20201017-League-of-Legends-Script\Dragontail Downloader\Dragontail\";
        static string baseUrl = @"https://ddragon.leagueoflegends.com/cdn/dragontail-{version}.tgz";
        static async Task Main(string[] args)
        {
            string archivePath, extractedPath;
            WebClient client = new WebClient();

            client.DownloadProgressChanged += Client_DownloadProgressChanged;

            archivePath = Path.Combine(downloadPath, "dragontail.tgz");
            extractedPath = Path.Combine(downloadPath, "dragontail");

            if (File.Exists(archivePath)) File.Delete(archivePath);
            if (Directory.Exists(extractedPath)) Directory.Delete(extractedPath, true);

            string versionsString = client.DownloadString("https://ddragon.leagueoflegends.com/api/versions.json");

            dynamic versionsJson = JsonConvert.DeserializeObject(versionsString);
            string lastVersion = versionsJson.First;

            Console.WriteLine("Downloading from " + baseUrl.Replace("{version}", lastVersion));
            await client.DownloadFileTaskAsync(baseUrl.Replace("{version}", lastVersion), downloadPath + "dragontail.tgz");
            Console.WriteLine("Downloaded");

            Console.WriteLine("Extracting");
            ExtractTGZ(downloadPath + "dragontail.tgz", Path.Combine(downloadPath, "dragontail"));
            Console.WriteLine("Extraction completed");

            Console.ReadLine();
        }

        static void Client_DownloadProgressChanged(object sender, DownloadProgressChangedEventArgs e)
        {
            Console.Write("\rDownloading\t{1}/{2}MB ({0}%)", e.ProgressPercentage, e.BytesReceived / (1024 * 1024), e.TotalBytesToReceive / (1024 * 1024));
        }

        static void ExtractTGZ(String gzArchiveName, String destFolder)
        {
            Stream inStream = new StreamReader(gzArchiveName).BaseStream;
            Stream gzipStream = new GZipInputStream(inStream);
            StreamWriter fileContents;

            using (var tarInputStream = new TarInputStream(gzipStream))
            {
                TarEntry entry;
                while ((entry = tarInputStream.GetNextEntry()) != null)
                {
                    if (entry.Name == null || entry.Name == "/") continue;
                    string path = Path.Combine(destFolder, entry.Name);
                    if (entry.IsDirectory)
                    {
                        Directory.CreateDirectory(path);
                        continue;
                    }

                    fileContents = new StreamWriter(path);
                    tarInputStream.CopyEntryContents(fileContents.BaseStream);
                    fileContents.Close();
                }
            }
        }
    }
}
