# Instructable

Een instructable is een stappenplan - zonder verdere uitleg - hoe je vertrekkend van de bill of materials en gebruik makend van de technische tekeningen de robot kan nabouwen. Ook de nodige stappen om de microcontroller te compileren en te uploaden staan beschreven.  

### stap 1
bestel alle componenten uit de bill of materials  

### stap 2
Test aan de hand van de proof of concepts alle componenten. Indien nodig kan je de datasheets raadplegen

### stap 3
Nu gaan we de printplaat solderen. Doe dit met behulp van het elektronisch schema. Zorg er ook altijd voor dat het solderen op een veilige manier kan.

### stap 4
Het chassis kan je 3D printen of je kunt ook zelf creatief te werk gaan. De bestanden hiervoor vind je terug onder technische tekeningen => mechanisch. De wielen zijn aangekocht die passen op de metal gear motoren.

### stap 5
Om alles te monteren kun je verschillende middelen gebruiken, je kunt het vast lijmen of vijzen gebruiken. Eerst worden de motoren met de motor mounts vastgezet. Hierna wordt de batterij houder gemonteerd. Soldeer de schakelaar tussen de voeding van de batterijen. Het monteren van de printplaat gebeurd rechtstreeks op het ge-3Dprinte chassis. Plaats de sensor naar wens in gaatjes die hiervoor voorzien zijn (tip: plaats de sensor liever dichter bij de wielen plaats van er verder af). Later kan je dan de afstand van de sensor ten opzichte van de wielen nog aanpassen om jouw linefollower te optimaliseren.

### stap 6
Upload het programma naar jouw arduino nano. Het programma kan je vinden onder code => finaal. Let er op dat de HC-05 module tijdens het uploaden niet verbonden is met de arduino.

### stap 7
Het instellen van de parrameters gebeurd via bluetooth of via USB in de app "Serial Bluetooth Terminal". De parrameters die ik gebruikt heb zijn terug te vinden bij gebruiksaanwijzing => setting.

Als je al bovenstaande stappen hebt volbracht is jouw auto klaar voor gebruik.
