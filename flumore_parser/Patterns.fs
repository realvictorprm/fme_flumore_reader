module Patterns
open FSharp.Text

(*  All patterns are static available and whitespace resitent. It's only at least one whitespace between each argument required. *)
        
// Pattern for the date format in the file
[<Literal>]
let patternDate = "(?<date>[0-9]{2}.[0-9]{2}.[0-9]{4}-[0-9]{2}:[0-9]{2})"
        
// Pattern for the identification numbers like the variant number and the counter number
[<Literal>]
let patternIdentification = "N(?<variant>[0-9]{3})-P(?<counter>[0-9]{3})"
        
// Pattern for the timesteps. Only usable in combination
[<Literal>]
let patternTimesteps = @"\s+(?<timesteps>[0-9]{3})\s+"
        
// Pattern for the date in the filename. It's different compared to the first date pattern!
[<Literal>]
let patternFileDate = "(?<date>[0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{2})"
        
// Complete filename pattern for matching the filename to it's various components
[<Literal>]
let patternFilenameComplete = "(?<simulationKind>[VS])-" + patternFileDate + "." + patternIdentification

// Complete file header pattern for matching the header to it's various components
[<Literal>]
let patternFirstLineIdentifier = 
    @"FLUMORE\s+(?<date>\d{2}\.\d{2}\.\d{4}\-\d{2}\:\d{2})\s+(?<timesteps>\d{1,3})\s+N(?<variant>\d{3})\-P(?<counter>\d{3})"
        
// Complete subheader pattern for matching the subheaderto it's various components
[<Literal>]
let patternTimestampIdentifier =
    @"\s?"+ patternDate + @"\s+(?<timestampKind>[SIM|VHS|SZO]{3})\s+(?<subspanCount>\d{1,5})\s+(?<_2DCount>\d{1,5})"
        
[<Literal>]
let patternSubSpanIdentifier = @"\s+(?<objKind>(?<ub>Ueberstr\.)|Bresche|Deichentl\.|Folgebruch|Innere Entl\.|(?<sm>Linien-SM|Punkt-SM))\s+(?<nlp>\d{1,5})(?(?<=\b1)\s+(?<rw>\d{1,10}\.\d)\s+(?<hw>\d{1,10}\.\d))\s+(?(ub)(?<hm>\d{1,10}\.\d*)\s+(?<qq>\d{1,10}\.\d*)|(?(sm)(?<smw>\d{1,10}\.\d*)|(?<bb>\d{1,10}\.\d*)\s+(?<btm>\d{1,10}\.\d*)\s+(?<q>\d{1,10}\.\d*)))"

[<Literal>]
let patternSubDataIdentifier = @"\bTeilbereich\s+(?<count>\d{1,10})\s+TB(?<id>\d{3})\-V(?<version>\d{2})"

[<Literal>]
let patternRowCSV = @"\s*(?<id>\d+)\s*\,\s*(?<x>\d+\.\d+)\s*\,\s*(?<y>\d+\.\d+)\s*\,\s*(?<z>\d+\.\d+)\s*\,\s*(?<wsp>\d+\.\d+)\s*\,\s*(?<h>\d+\.\d+)\s*\,\s*(?<vres>\d+\.\d+)\s*"

type FirstLineIdentifierRegex = RegexProvider.Regex<patternFirstLineIdentifier>
let completeIdentifierRegex = FirstLineIdentifierRegex()

type FilenameRegex= RegexProvider.Regex<patternFilenameComplete>
let filenameRegex = FilenameRegex()

type TimestampRegex = RegexProvider.Regex<patternTimestampIdentifier>
let timestampRegex = TimestampRegex()
       
type SubSpanRegex = RegexProvider.Regex<patternSubSpanIdentifier>
let subspanRegex = SubSpanRegex()

type SubDataIdentifierRegex = RegexProvider.Regex<patternSubDataIdentifier>
let subdataIdentifierRegex = SubDataIdentifierRegex()

type RowCSVRegex = RegexProvider.Regex<patternRowCSV>
let csvRegex = RowCSVRegex()