# Paper Mario PAL v0.1-pal-alpha — Installation (Deutsch)

Für **Miyoo Mini Plus mit OnionOS**. Dies ist eine fertig installierbare native ARM-Portierung: **Keine Kompilierung nötig.** Du benötigst deine eigene unveränderte europäische PAL-Kopie von Paper Mario. ROM und extrahierte Spielassets sind nicht enthalten; ein ROM-Download wird nicht angeboten.

## Herunterladen und installieren

1. Wähle auf der [Release-Seite von SunnyBoon/miyoo64](https://github.com/SunnyBoon/miyoo64/releases) **v0.1-pal-alpha** und lade **PaperMarioPAL-0.1-pal-alpha-OnionOS.zip** herunter. Prüfe die SHA-256 mit der beiliegenden SHA256SUMS: `763c582d37bff03363a14b2d04fe0f3726f44354f3e1098bef6c83302ca4f794`. Lade außerdem `LICENSE`, `NOTICE.md` und `THIRD-PARTY-LICENSES.txt` herunter und bewahre sie zusammen mit dem ZIP auf.
2. Entpacke den ZIP-Inhalt in das **Wurzelverzeichnis der SD-Karte** und erhalte `App/PaperMarioPAL/`. Dort müssen sechs App-Dateien nebeneinander liegen: `pm64`, `recipes.bin`, `launch.sh`, `config.json`, `icon.png`, `full_resolution`. Keine zusätzliche äußere Ordnerebene anlegen. Falls `App/PaperMarioPAL/` bereits existiert, sichere und prüfe die vorhandene Installation, bevor du etwas ersetzt.
3. Lege deine eigene **Paper Mario (Europe, PAL)**-ROM als **`App/PaperMarioPAL/rom.z64`** ab. Erforderlich ist eine **64-MiB-Datei (67.108.864 Bytes) im Big-Endian-Z64-Format** mit SHA-1 **`2111d39265a317414d359e35a7d971c4dfa5f9e1`**. US-/Japan-Versionen, veränderte Dateien oder eine falsche Byte-Reihenfolge passen nicht. Umbenennen wandelt den Dateiinhalt nicht um.
4. Setze die SD-Karte wieder in den Miyoo Mini Plus ein, öffne die Anwendungsliste in OnionOS und starte **Paper Mario PAL 0.1-pal-alpha**.
5. Beim ersten Start prüft die App die ROM und erzeugt benötigte Daten lokal unter **`App/PaperMarioPAL/data/`**. Warte, bis dies abgeschlossen ist. Du musst keine Assets manuell extrahieren und nichts kompilieren. Die ROM bleibt für laufende Datenzugriffe auf der Karte. Keine alten Caches oder Save-States in diesen Ordner kopieren.
6. Wähle im Sprachmenü **English, German, French oder Spanish**. Folge den Hinweisen des Spiels. Verwende die regulären Spielstände dieses Builds und neu angelegte Save-States; eine Migration von Zuständen zwischen Builds wird nicht unterstützt.

## Fehlerdiagnose

- **App fehlt oder startet nicht:** Prüfe die genaue Ordnerstruktur und alle sechs Dateien. `pm64` und `launch.sh` müssen ausführbar bleiben; das ZIP speichert dafür 0755, für die übrigen Dateien 0644. Falls das Entpackprogramm fragt, erhalte die Dateirechte. Verwende das vollständige zusammengehörige ZIP.
- **ROM fehlt oder wird abgelehnt:** Prüfe den genauen Ablageort von `rom.z64`, Größe, Big-Endian-Format und SHA-1 oben. Vermeide eine doppelte Dateiendung. Verwende deinen eigenen passenden Dump; hier gibt es keinen Downloadlink.
- **Erster Start dauert länger:** Lass die lokale Datenerzeugung fertig werden. Die SD-Karte muss beschreibbar sein und freien Speicher haben. Unterbrich keinen Schreibvorgang und übernimm keinen Cache eines anderen Builds.
- **Speichern/Laden klappt nicht, Ton fehlt oder der Start scheitert weiterhin:** Prüfe Gerät/OnionOS und ZIP-Integrität und behalte das originale Binär-/Rezeptpaar zusammen. Schütze vorhandene Spielstände. Notiere die sichtbare Fehlermeldung und Schritte für eine Supportmeldung; ersetze keine Dateien durch andere Builds und teile keine ROMs, Spielstände oder privaten Daten.

## Testumfang und bekannte Fehler

Das **exakte Endnutzer-ZIP bestand 5/5 manuelle Benutzer-Hardwareprüfungen**: Start aus diesem App-Ordner, alle vier Sprachen, Hauptmenü/neuer Spielstand, Intro/erster Kampf, reguläres Speichern/Laden und Save-State/Load-State. Die M14.39b-Grundlage wurde außerdem stabil durch den Prolog bis zum Anfang von Kapitel 1 gespielt. Dies ist eine **Alpha**, keine vollständige Spielvalidierung; spätere Szenen und alle Spezialfähigkeiten sind nicht umfassend getestet. Nur das genannte Gerät/System ist durch diesen Nachweis abgedeckt.

Bekannte, noch nicht behobene Grafikfehler:

- An einigen großen Bäumen erscheinen schmale, gerade Segmentnähte, durch die der Hintergrund sichtbar wird.
- Bei Goombarios vergrößerter Gegnerdarstellung ist der Rahmen unvollständig: zwei statt vier Seiten.
- Ein Sternensplitter blinkt fehlerhaft, obwohl er statisch sichtbar bleiben sollte; sein Schatten bleibt sichtbar, während der Sternensplitter verschwindet.

Der separate [Upstream-PR #7](https://github.com/mr-finster/miyoo64/pull/7) besitzt seine eigene **8/8**-Hardwareabnahme und andere Quellen, Programm- und Rezeptdateien. Diese ersetzt nicht die **5/5**-Abnahme dieses ZIPs. Installationen und Zustände/Caches nicht vermischen. Beachte KNOWN-ISSUES.md und NOTICE.md.
