# 1. Protokoll | Uller Lucas | AHME17 

* **Thema:** Versionsverwaltungssystem, Linux-Befehle
* **Datum:** 30.09.2019
* **Gefehlt:** --
* **Erstellt von:** ulllum17
* **Protokoll nächste Einheit:**
--------------------------------------------------------------------------
## Inhaltsverzeichnis

1. [Versionsverwaltungssystem](#versionsverwaltungssystem)
    1. [Absatz](#absatz)
    1. [Zeilenumbruch](#zeilenumbruch)
    1. [Überschrift](#überschrift)
    1. [Hervorheben](#hervorheben)
    1. [Code](#code)
    1. [Text Grau hinterlegen](#text_grau_hinterlegen)
    1. [Zitat](#zitate)
    1. [Links](#links)
        1. [Inline](#inline)

--------------------------------------------------------------------------

## Versionverwaltungssystem

Hier einige wichtige Beispiel Syntax für die Formatierung bei Markdown.

### Absatz
Absätze werden mit einer leer Zeile erzeugt.

    Absatz 1

    Absatz 2
    
### Zeilenumbruch

    Text....    
    Text....    
    Text....

### Überschriften
Es sind **maximal 6** Überschiftsstufen möglich!

    # Überschrift 1
    ## Überschrift 2
    ### Überschrfit 3
    #### Überschrift 4
    ##### Überschrift 5
    ###### Überschrift 6


### Hervorheben
Um wichtiges hervorzuheben gibt es x möglichkeiten

    **Fett**
    *Kursiv*
    ***Fett-und-Kursiv***
    ~~Durchgestrichen~~

### Code
Um zu Kennzeichnen das es sich um Computer code    
Mit dem 

 ```C
    ``` <Computersprache>
    int main(){
    printf("Hello World!");
    return 0;
    }
    ```
```

### Text Grau hinterlegen
Um etwas grau zu hinterlegen zweimal die **Tabulatortaste** drücken

### Zitat
Zitate werden mit **>** eingeleitet 

    > Zitat
   
### Links
Es gibt zwei Möglichkeit um Links einzufügen
* inline 
* Referenz

#### Inline   

     [Anzeige_Name1](https://test.org)
     [Anzeige_Name2](https://test.org "POPUP Fenster, wenn mit der Maus auf dem Link")

*Veranschaulichung des Popup:*    
[Anzeige_Name1](https://test.org)  
[Anzeige_Name2](https://test.org "POP UP Fenster, wenn mit der Maus auf dem Link")

#### Referenz   

    [Anzeige_Name][1]
    [1]:https://test.org "POP UP Fenster, wenn mit der Maus auf dem Link"
    
**Vorteile**
  * Links *können* gebündelt z.B. am Ende zusammengefasst werden
