# Gebruiksaanwijzing

### opladen / vervangen batterijen
De batterijen die gebruikt worden zijn 2 18650 li-ion batterijen. Deze worden opgeladen met een geschikte oplader. Om de batterijen te vervangen moet eerst de PCB van de auto worden verwijderd. Hierna kunnen de batterijen uit de houder worden gehaald en opgeladen.

### draadloze communicatie
#### verbinding maken
Voor de draadloze communicatie wordt er gebruik gemaakt van een HC-05 bluetooth module. Op de laptop wordt de app "Serial Bluetooth Terminal" gebruikt van Arduino om verbinding te maken met de module. Hiermee kunnen de commando's worden doorgestuurd naar de Arduino. Voor verbinding te maken ga je eerst naar Bleutooth instellingen van je laptop, daar zou je dan de HC-05 module moeten terugvinden. Wanneer je wilt verbinden zul je een code moeten invullen wat meestal "1234" of "0000" is. Van af je verbonden bent zal je laptop automatisch 2 comporten aanmaken let er wel op maar 1 van de 2 zal werken om commando's door te sturen. Je kunt deze comporten ook terug vinden bij Apparaatbeheer onder ports.

#### commando's
debug [on/off] => Opvragen van de ingestelde parrameters.

run => Starten van de LineFollower.

stop => Stoppen van de LineFollower.

set cycle [µs] => Instellen van de cycle time. Deze is ongeveer het dubbele van de calculation time.

set power [0..255] => Instellen van de power van de motoren.

set diff [0..1] => Door de diff aan te passen zal de auto vertragen of versnellen in de bochten. Hoe hoger diff hoe meer de auto versnelt in de bochten.

set kp [0..] => Dit is de corrigatie van de fout. Hoe hoger kp hoe sterker de auto zich corrigeert om de zwarte lijn in het midden van de sensor te hebben. Als deze te hoog is zal de auto schoken van links naar rechts over de lijn.

set ki [0..] => Hoe langer de error duurt hoe sterker de linfollower zal bijsturen.

set kd [0..] => Dit is de foutverandering. Deze kijkt naar de fout uit de vorige cyclus en gaat dan de fout bijregelen. Was deze fout groot dan wordt er meer gecorigeerd.

calibrate black => Hierdoor worden de zwarte waarden van de sensor gecalibreert.

calibrate white => Hierdoor worden de witte waarden van de sensor gecalibreert.

### kalibratie
Het kalibreren van de senor gebeurd met de commando's "calibrate black" en "calibrate white". Om de zwarte waarden te kalibreren moet de sensor boven een zwart oppervlak staan en dan het commando ingeven. Om de witte waarden te kalibreren gebeurd het zelfde maar op een wit oppervlak.

### settings
De robot rijdt stabiel met volgende parameters: Diff: 0.85, Kp: 22, Ki: 0.60, Kd: 0.50, Cycle 3000.

### start/stop button
Er is een start/stop button voorzien op de robot. Deze bevindt zich op de PCB, de knop is aangesloten op een intterupt pin (pin2). Zorg er natuurlijk wel eerst voor dat de robot gekalibreert is anders zal hij maar weinig doen.
