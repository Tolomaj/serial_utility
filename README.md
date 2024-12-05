# Úvod
Toto je aplikace na čtení sériového portu která odchytává flagy před správou a zvýrazňuje příslušnou barvou. \
Také má funkce pro hledání v záznamech nebo spuštění externích aplikací.

![preview](http://192.168.34.21:3000/ogross/serial_util/raw/branch/master/docs/ftest.png)

# Obsah

### Složka src: *(samotná aplikace)*

|      file      |    description                                   |
|----------------|--------------------------------------------------|
|asinc_timer.hpp | nepřesné časování pro kumulaci zpráv             |
|colors.hpp      | knihovna pro barvy v terminálu                   |
|executer.hpp    | spouštění příkazů mimo aplikace                  |
|help_file.hpp   | výpis příkazu help                               |
|ini.hpp         | ukládání do souboru ini                          |
|main.cpp        | hlavní smyčka + rozšířené příkazy + testy        |
|serial.hpp      | ovládání seriové linky                           |
|settings.hpp    | globální objekt nastavení a globální enumy       |
|terminal.hpp    | Přijímání std příkazů a zobrazování do terminálu |

### Složka stm_lib: *(knihovna pro komunikaci z stm32 (není třeba k používání samotné aplikace))*
|      file      |    description                                   |
|----------------|--------------------------------------------------|
|debug.hpp       | knihovna pomáhající s formátování zpráv z stm    |
>⚠️ Knihovna pro stm není ještě hotová (TODO)



# Funcke programu

Program čte a řeší seriové porty.*(příkazi v help)* \
Přijaté správy zvýrazňuje podle priority pokud jí zpráva má. Také zvýrazňuje některé keywordy jako jsou čísla , "error" nebo '!' na konci řádku. \
Pomocí příkazu se dá v zprávách vyhledávat regulárními výrazi. \
Také program umožňuje spouštět příkazy přímo z něj a řeší zavírání a otevírání Seriové linky.


## Formáty zpráv na které program reaguje: 

- [PRIORITY]Message\n\r
- [PLACE]Message\n\r
- [PRIORITY][PLACE]Message\n\r
- [PLACE][PRIORITY]Message\n\r
- Message\n\r

Pokud zpráva neobsahuje *[PRIORITY]* je jí přidělená zakladní. (na základě nastavení)\
Možné priority zpráv:
 **ERROR**,**INFO**,**DEBUG**,**WARNING**,**NORMAL**,**OK**,**SILENT**,**TEST**

*[PLACE]* může být jakákoli souborová cesta s číslem řádku, nejlepší je však používat makro \_\_FILE\_\_:\_\_LINE\_\_

Ukázka správných formátů zpráv:

  - "[OK][main.cpp:5]helo_world!\n[~]this is folowing line 1 \n[~]this is folowing line 2 \n"
  - "[ERR][main.cpp:15]\n"
  - "[KEdada.z:152] onyl file test\n"
  - "[C:\\Users\\ogross\\Desktop\\panel_w_editor\\src\\uproc03\\fonts.cpp:1328] fullpath test\n"
  - "some text ERROR\n"
  - "[DEBUG] debug\n"
  - "[BEGIN] zacatek sekce\n"
  - "[WARNING] warning\n"
  - "[NORMAL] normal\n"
  - "[OK] ok\n"

> ℹ️ většina priorit mění jen barvu a kategorii zprávy, ovšem [BEGIN] je výjimkou. [BEGIN] při přijetí nuluje hodiny a je zamýšleno na testování jak dlouho nějaký tásk trvá.

Nahlédnout do toho jak zprávy vypadají můžete v programu pomocí příkazu **test**

## Spouštení zkrze program

Pomocí příkazu **make** nebo **exec** lze spouštět externí programy. \
Protože některé příkazy potřebují volnou seriovou linku v settings.ini lze definovat příkazy před jejiž provedením se seriová linka uvolní.

Příklad řádku v nastavení(.ini soubor):
```
serial_commands=make upload,nahrej,make flash
```

----------------------------------------------------------

## Příkazy programu 
> ℹ️ stejný text vypíšeme i příkazem help

```
----- Manual pro program -----
Prikazi v modu terminalu:

                     help - Zobrazí pomoc ovládání programu.

                 hint off - Zapne doporučování příkazů.
                     hiof - Je aliasem hint off.

                  hint on - Vypne doporučování příkazů.
                     hion - Je aliasem hint on.

                     quit - Zastaví program.
                        q - Je aliasem quit.

                     scan - Vypíše dostupné porty.

               open [...] - Otevre port.(dovolene formaty jsou napr: 'COM3' '3')..
                  o [...] - Je aliasem open.

                    close - Zavře port.
                        c - Je aliasem close.

                   reopen - Restartuje seriový port.
                        r - Je aliasem reopen.

                     port - Vypíše aktuálně otevřený port.
                        p - Je aliasem port.

               send [...] - Pošle znaky na seriovou linku.
                  s [...] - Je aliasem send.

              direct_mode - Přepne program do módu kde odesílá vše z klavesnice po seriové lince. Z tohoto módu lze odejít zmáčknutím ESC.
                       dp - Je aliasem direct_mode.
                       dm - Je aliasem direct_mode.

                     halt - Dočasně zastaví seriovou linku.

                    clear - Vymaže přijaté zprávy.
                       cl - Je aliasem clear.
                      cls - Je aliasem clear.

              reset_clock - Restartuje hodiny příchodu zpráv.
                   rstclk - Je aliasem reset_clock.
                       rc - Je aliasem reset_clock.

                     make - Spustí příkaz make. A vypisuje výstup.
               make [...] - Je aliasem make.

               exec [...] - Spustí příkaz. A vypisuje výstup.
                  e [...] - Je aliasem exec.

                 ee [...] - Spustí příkaz. A vypisuje výstup(po dokončení se zastaví nehledě na nastavení).

               find [...] - Filtruje zpravy regulárním výrazem.
                  f [...] - Je aliasem find.

               file [...] - Filtrujeme jména souboru regulárním výrazem.
                  l [...] - Je aliasem file.

             reset_filter - Restartujeme filtry regulárních výrazů.
                rstfilter - Je aliasem reset_filter.
                       rf - Je aliasem reset_filter.

               save [...] - Uloží aktuální zprávy do souboru.(neukládá čas příchodu).
                 sv [...] - Je aliasem save.

               load [...] - Načte soubor výpisu z seriové linky.
                 ld [...] - Je aliasem load.

                 load_log - Načte zprávy které byli v aplikaci při posledním spuštění.
                       ll - Je aliasem load_log.

           highliting off - Vypne zvýrazňování v textu (ERROR se zvýrazní červeně)(linka na konci s ! dostane warning prioritu pokud žádnou nemá).
                      hof - Je aliasem highliting off.

            highliting on - Zapne zvýrazňování v textu (ERROR se zvýrazní červeně)(linka na konci s ! dostane warning prioritu pokud žádnou nemá).
                      hon - Je aliasem highliting on.

         folowing_sign on - Zapne znaménko před folower linkou.
                   flwron - Je aliasem folowing_sign on.

        folowing_sign off - Vypne znaménko před folower linkou.
                   flwrof - Je aliasem folowing_sign off.

                 time off - Vypne zobrazování času příchodu.
                      tof - Je aliasem time off.

                  time on - Zapne zobrazování času příchodu.
                      ton - Je aliasem time on.

        precision_time on - Nastaví formát času na MILLISECONDS
                      pto - Je aliasem precision_time on.

       precision_time off - Nastaví formát času na HOD:MIN:SEC
                      ptf - Je aliasem precision_time off.

         file_location on - Zobrazí soubor a řádek logu.
                      fon - Je aliasem file_location on.

        file_location off - Skryje soubor a řádek logu.
                      fof - Je aliasem file_location off.

         file_shorting on - Zobrazí pouze jméno souboru bez cesty.
                     fson - Je aliasem file_shorting on.

        file_shorting off - Zobrazí i celou cestu pokud jí soubor má.
                     fsof - Je aliasem file_shorting off.

        priority_color on - Zapne barvy priorit.
                     pcon - Je aliasem priority_color on.

       priority_color off - Vypne barvy priorit.
                     pcof - Je aliasem priority_color off.

            decoration on - Zapne výchozí dekorace logů.
                      don - Je aliasem decoration on.

           decoration off - Vypne veškeré dekorace logů.
                      dof - Je aliasem decoration off.

               head [...] - Nastaví minimální odsazení řádků.
                 of [...] - Je aliasem head.

                 hide all - Skryje všechny typy zpráv.
                      hal - Je aliasem hide all.
                     hall - Je aliasem hide all.

                 show all - Zobrazí všechny typy zpráv.
                      sal - Je aliasem show all.
                     sall - Je aliasem show all.

               show [...] - Ukáže typ zprávy. Možné typy zpráv: normal,debug,info,warning,error,ok,silent,testok,testnok,all a jejich aliasi: n,dbg/d,i,wrng/w,err/e,k,sil/s,tok/tk,ton/tn.
                 sh [...] - Je aliasem show.

               hide [...] - Skryje typ zprávy. Možné typy zpráv: normal,debug,info,warning,error,ok,silent,testok,testnok,all a jejich aliasi: n,dbg/d,i,wrng/w,err/e,k,sil/s,tok/tk,ton/tn
                 hi [...] - Je aliasem hide.

          show_only [...] - Ukáže jeden typ zprávy. Možné typy zpráv: normal,debug,info,warning,error,ok,silent,testok,testnok,all a jejich aliasi: n,dbg/d,i,wrng/w,err/e,k,sil/s,tok/tk,ton/tn.        
                 so [...] - Je aliasem show_only.

          hide_only [...] - Skryje jeden typ zprávy. Možné typy zpráv: normal,debug,info,warning,error,ok,silent,testok,testnok,all a jejich aliasi: n,dbg/d,i,wrng/w,err/e,k,sil/s,tok/tk,ton/tn.       
                 ho [...] - Je aliasem hide_only.

               show reset - Ukáže zprávy které jsou ve výchozím nastavení vyditelné.
                       sr - Je aliasem show reset.
                      sor - Je aliasem show reset.

      show_return_zero on - Automaticky dokončit process který skončil návratovou hodnotou 0.
                      srz - Je aliasem show_return_zero on.

     show_return_zero off - Zastavit se po dokončení programu i pokud skončil návratovou hodnotou 0.
                      hrz - Je aliasem show_return_zero off.

 use_confirm_character on - Po potvrzení čekací obrazovky se potvzovací znak i zapíše do příkazu.
                     ucch - Je aliasem use_confirm_character on.

use_confirm_character off - Po potvrzení čekací obrazovky se potvzovací znak zahodí.
                     dcch - Je aliasem use_confirm_character off.

                    reset - Resetuje veškeré filtry, jak souboru tak textu tak priority.
                      rst - Je aliasem reset.
                       rs - Je aliasem reset.

 undefined_priority [...] - Nastaví výchozí prioritu pro zprávy bez priority.
               ndef [...] - Je aliasem undefined_priority.

          save_on_quit on - Obnoví ukládání nastavení na konci programu.
                  do_save - Je aliasem save_on_quit on.

         save_on_quit off - Zamezí na konci programu uložení nastavení.
                dont_save - Je aliasem save_on_quit off.
                       ds - Je aliasem save_on_quit off.

            save_settings - Uloží aktuální nastavení.
                      sas - Je aliasem save_settings.

                    speed - Ukáže rychlost seriového portu
                    bound - Je aliasem speed.
                       bd - Je aliasem speed.

              speed [...] - Nastaví rychlost seriového portu
              bound [...] - Je aliasem speed.
                 bd [...] - Je aliasem speed.

                     ping - Response text test

                     test - Print test

                    ftest - Folower test
```

----------------------------------------------------------

## Spuštění s příkazy.

Pokud chceme spouštět program s konkrétním nastavením použijeme flag -c [command] kde command je příkaz z sekcec **Příkazy programu**.

```
sut.exe -c "head 20" -c sogof -c "open 10"
```
Například toto spustí program s hlavičkou 20 znaků, otevře port 10 a žádné zmeny v nastavení nebude ukládat.

> ⚠️ pokud je nějaký příkaz neplatný tak se provede bez nahlášení neuspěchu. \
> 🤓 Např. pokud se pokusíme otevřít neexistující port. Otevře se prostě ten z nastavení.

# Kompilace & spouštění

Kompilace programu je pomocí příkazu:
```
make
```

Spustění programuje pomocí příkazu:

```
make run
```

# Vytvářené složky a soubory

## %appdata%/serial_utility 
Složka obsahující veškerá data data programu (jinde si program nic neukládá).
Tato složka je i spolu se soubory v ní vytvářena při strtu/konci programu.


## %appdata%/serial_utility/last_log
Seznam zpráv které byli načtené v programu ,na posledy co byl otevřený. 
Zprávy jsou uloženy v speciálním formátu zahrnující i čas kdy zpráva dorazila.

## %appdata%/serial_utility/settings.ini
Nastavení aplikace.




----------------------------------------------------------
----------------------------------------------------------
----------------------------------------------------------
----------------------------------------------------------

# TODO ZALEŽITOSTI
- ?? možná přidat ANY: něco  (jako flag) ale přijde mi dost obecný a né uplně použitelný

### TODO fičury:  
 - přidat "\n\r" jen přidat volný řádek (otázkou jestli je potřeba když máme [~])
 - ?? přidat zobrazování proměnných 
 - !! testování !!

### TODO pro stm32:
//stm32 knihovna pro debug (kompatibilní s klasickým debugem)
// přidat nastavení výpisu cesty
- var(name,value); // vypíše terminálu proměnou ??možná fičura
- softbreakpoint(); // čeká na input z klávesnice

### BUGS:
   - odeílání šípek je jiné než putty
   - při zápisu na vynutý modul program zamrzne(po ukončení q) dokud není modul zapnutý (problém v driveru)
