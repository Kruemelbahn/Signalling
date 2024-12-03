# Signalling

...ist eine kleine Sammlung von Signalansteuerungen (mit analoger Ansteuerung) auf meiner Modellbahn:

- Ne1_Zp11 zeigt die Hard- und Software für das Nebenbahnsignal Ne1 mit der Kennleuchte für Zp11 (siehe https://kruemelsoft.hier-im-netz.de/ne1.htm)
- Zs2_Zs3 zeigt die Hard- und Software für die Signale Zs2 und Zs3. Beide Signale basieren auf den Signalen von https://smf-modelle.de/ und wurden um eine Hard- und Software für vorbildgerechtes Auf- und Abblenden der Signalbilder erweitert.
- Bue zeigt die Hard- und Software für Signale an einem Bahnübergang: Wechselblinken für die Andreaskreuze und einfaches Blinken für das Überwachungssignal Bü1.
  Diese Software gibt es in zwei Versionen:
  - mit Ein-Aus-Schalter und 
  - mit einem Ein-Taster und automatischem Ausschalten nach Zeit (aktuell ca. 15s)
- die Ansteuerung eines (analog gesteuerten) Hauptsignal (optional mit Vorsignal und Blocksteuerung) steht hier [Signal-Z](http://www.github.com/Kruemelbahn/Signal-Z) zur Verfügung

Der Quellcode wird nur benötigt, wenn
- nan neugierig ist
- oder den Quellcode ändern und somit neu kompilieren möchte.
Zum Kompilieren wird ein AVR-Compiler (z.B. das ATMEL-Studio) benötigt.

Ansonsten reichen die Hex-Dateien völlig aus, diese werden in den ATtiny13 geflasht.
Die Software auf dem ATtiny13 läuft mit den Default-Fuses: low=0x6A high=0xFF.
