module Parser
open System
open Definitions
open Patterns
open Utils
open System.Diagnostics
open System.Threading.Tasks

type TimestampPaket = {
    Identifier : TimestampIdentifier
    Kind : SituationIdentifier
    Data : (int * float * float * float * float * float * float) array
}

/// <summary>
/// Tries to parse the first line of a simulation file.
/// It's allowed to pass the complete file text here because the pattern is whitespace resistent and unique.
/// </summary>
/// <param name="firstLine">The content of the first line.</param>
/// <returns>PackageIdentifier record specifying all valuable information about the file.</returns>
let tryParsePackageIdentifier firstLine =
    maybe {
        let! regex = 
            let res = completeIdentifierRegex.TypedMatch(firstLine)
            if res.Success then Some res
            else None
        let! date = regex.date.Value |> DateTime.tryParse |> Option.map DateTime.toUniversalTime
        let timesteps = regex.timesteps.Value |> Convert.ToInt32
        let variantNumber = regex.variant.Value |> Convert.ToInt32
        let counterNumber = regex.counter.Value |> Convert.ToInt32
        return PackageIdentifier(
            date,
            timesteps,
            variantNumber,
            counterNumber,
            regex.Index,
            regex.Length + regex.Index)
    }
            
/// <summary>
/// 
/// </summary>
/// <param name="filename"></param>
let tryParseFileName filename =
    maybe {
        let! regex = 
                let res = filenameRegex.TypedMatch(filename)
                if res.Success then Some res
                else None
        let simulationKind, date, variant, counter =
            regex.simulationKind.Value, regex.date.Value, regex.variant.Value, regex.counter.Value
        let! scenario = 
            match simulationKind |> char with
            | 'V' -> Some SimulationKind.Prediction
            | 'S' -> Some SimulationKind.Scenario
            | _ -> None
        let! date = date |> DateTime.tryParse |> Option.map DateTime.toUniversalTime
        let variantNumber, counterNumber = 
                variant |> Convert.ToInt32, counter |> Convert.ToInt32
        return FileIdentifier(
                Kind = scenario,
                Counter = counterNumber,
                Variant = variantNumber,
                Created = date )
    }
    
/// <summary>
/// Tries to parse the timestamp identifier and extract the valuable information from it.
/// Keep in mind that the parsing requires that the timestamp is the first content in the string and not the first line identifier.
/// If this isn't the case this WILL FAIL.
/// </summary>
/// <param name="timestamp">String containing the timestamp from beginning on.</param>
let tryParseTimestampIdentifier timestamp =
    maybe{
        let regex = timestampRegex.TypedMatch(timestamp)
        let! timestampKind = 
            maybe {
                match regex.timestampKind.Value with
                | "SIM" -> return TimestampKind.Simulation
                | "VHS" -> return TimestampKind.Prediction
                | "SZO" -> return TimestampKind.Scenario
                | _ -> ()
            }
        let! date = regex.date.Value |> DateTime.tryParse |> Option.map DateTime.toUniversalTime
        let subspanCount = regex.subspanCount.Value |> System.Convert.ToInt32
        let _2DCount = regex._2DCount.Value |> System.Convert.ToInt32
        return TimestampIdentifier(
                PredictionDate = date,
                Kind = timestampKind,
                SubSpanCount = subspanCount,
                _2DCount = _2DCount )
    }
        
let tryParseSubHeaderIdentifier lines =
    maybe{
        let! regex = 
            let res = subspanRegex.TypedMatch(lines)
            if res.Success then Some res
            else None
        let! nlp = regex.nlp.Value |> Convert.tryToInt32
        match regex.objKind.Value with
        | "Bresche" -> 
            let! rw = regex.rw.Value |> Convert.tryToFloat
            let! hw = regex.hw.Value |> Convert.tryToFloat
            let! bb = regex.bb.Value |> Convert.tryToFloat
            let! btm = regex.btm.Value |> Convert.tryToFloat
            let! q = regex.q.Value |> Convert.tryToFloat
            return Definitions.SituationIdentifier.Bresche(rw, hw, Deichbresche( bb, btm, q))
        | "Ueberstr." ->
            let! hm = regex.hm.Value |> Convert.tryToFloat
            let! q = regex.qq.Value |> Convert.tryToFloat
            return Definitions.SituationIdentifier.Ueberstr(Deichueberstroemung(hm, q))
        | "Deichentl." ->
            let! rw = regex.rw.Value |> Convert.tryToFloat
            let! hw = regex.hw.Value |> Convert.tryToFloat
            let! bb = regex.bb.Value |> Convert.tryToFloat
            let! btm = regex.btm.Value |> Convert.tryToFloat
            let! q = regex.q.Value |> Convert.tryToFloat
            return Definitions.SituationIdentifier.Deichentl(rw, hw, Deichbresche(bb, btm, q))
        | "Folgebruch" ->
            let! rw = regex.rw.Value |> Convert.tryToFloat
            let! hw = regex.hw.Value |> Convert.tryToFloat
            let! bb = regex.bb.Value |> Convert.tryToFloat
            let! btm = regex.btm.Value |> Convert.tryToFloat
            let! q = regex.q.Value |> Convert.tryToFloat
            return Definitions.SituationIdentifier.Folgebruch(rw, hw, Deichbresche(bb, btm, q))
        | "Innere Entl." ->
            let! rw = regex.rw.Value |> Convert.tryToFloat
            let! hw = regex.hw.Value |> Convert.tryToFloat
            let! bb = regex.bb.Value |> Convert.tryToFloat
            let! btm = regex.btm.Value |> Convert.tryToFloat
            let! q = regex.q.Value |> Convert.tryToFloat
            return Definitions.SituationIdentifier.InnereEntl(rw, hw, Deichbresche(bb, btm, q))
        | "Linien-SM" ->
            let! minkhr = regex.smw.Value |> Convert.tryToFloat
            return Definitions.SituationIdentifier.LinienSM(minkhr)
        | "Punkt-SM" ->
            let! rw = regex.rw.Value |> Convert.tryToFloat
            let! hw = regex.hw.Value |> Convert.tryToFloat
            let! maxsh = regex.smw.Value |> Convert.tryToFloat
            return Definitions.SituationIdentifier.PunktSM(rw, hw, maxsh)
        | _ -> ()
    }
    
let tryParseSubDataIdentifier first =
    maybe{
        let! regex =
            let res = subdataIdentifierRegex.TypedMatch(first)
            Option.someIf res.Success res
        let! count = regex.count.Value |> Convert.tryToInt32
        let! id = regex.id.Value |> Convert.tryToInt32
        let! version = regex.version.Value |> Convert.tryToInt32
        return TimestampSubDataIdentifier(count, id, version)
    }

let (|IsTimestampHeader|_|) (lines:string array, lineNumber) =
    let content = lines.[lineNumber]
    tryParseTimestampIdentifier content
    
let (|IsTimestampSubHeader|_|) (lines:string array, lineNumber) =
    if lines.Length - 1 >= lineNumber + 3 then 
        let first, second, third = lines.[lineNumber], lines.[lineNumber + 1], lines.[lineNumber + 2]
        tryParseSubHeaderIdentifier (first + second + third)
    else None

let (|IsTimestampSubData|_|) (lines:string array, lineNumber) =
    let first = lines.[lineNumber]
    tryParseSubDataIdentifier first
    
let tryParseSimulationFile (path:string) =
    let ret = 
        maybe {
            #if DEBUG
            printfn "Path: %A" path
            #endif 
            let! allLines =
                File.tryReadAllLines path |> Result.asOption
            let! fileIdentifier = tryParseFileName (System.IO.Path.GetFileName path)
            let! packageIdentifier = tryParsePackageIdentifier (allLines.[0])
            return allLines, fileIdentifier, packageIdentifier
        }
    match ret with
    | Some (lines, fileIdentifier, packageIdentifier) ->
        let res = ResizeArray [| AST.Header packageIdentifier |]
        let rec parseFile lineNumber =
            if lineNumber < lines.Length then
                let newPos, newAst = 
                    let context = res.[res.Count - 1]
                    match lines, lineNumber with
                    | IsTimestampHeader identifier ->
                        lineNumber + 1, TimestampHeader(identifier, lineNumber)
                    | IsTimestampSubHeader identifier ->
                        printfn "Pre-Parser, INFO: Parsing %.2f %% complete" (((float lineNumber + 2.) / float lines.Length) * 100.)
                        lineNumber + 3, TimestampSubHeader(context, identifier, lineNumber, lineNumber + 2)
                    | IsTimestampSubData (TimestampSubDataIdentifier(count, _, _) as identifier) ->
                        lineNumber + 2 + count, TimestampSubData(context, identifier, lineNumber, lineNumber + 1 + count)
                    | _ -> 
                        printfn "Pre-Praser, WARNING: Parsing line %A with content \"%s\" failed!" lineNumber (lines.[lineNumber])
                        lineNumber + 1, Detached
                newAst |> res.Add
                parseFile newPos
            else () // Finished parsing
        parseFile 1 
        Some { AllLines = lines; AST = res |> Seq.toList }
    | _ -> 
        printfn "Tokenizing and Pre-Parsing failed." 
        None

let  testUnicodeNameaeae () = ()

/// C compatibility API
let getSimulationFileData path =
    let stopwatch = Stopwatch.StartNew()
    let res = 
        match tryParseSimulationFile path with
        | Some res ->
            let AllLines, ast = res.AllLines, res.AST
            let parseCSV startLine endLine =
                let parseLine nmbr =
                    maybe {
                        let line = AllLines.[nmbr]
                        let res = csvRegex.TypedMatch line
                        if res.Success then
                            let! id = res.id.Value |> Utils.Convert.tryToInt32
                            let! x = res.x.Value |> Utils.Convert.tryToFloat
                            let! y = res.y.Value |> Utils.Convert.tryToFloat
                            let! z = res.z.Value |> Utils.Convert.tryToFloat
                            let! wsp = res.wsp.Value |> Utils.Convert.tryToFloat
                            let! h = res.h.Value |> Utils.Convert.tryToFloat
                            let! vres = res.vres.Value |> Utils.Convert.tryToFloat
                            #if DEBUG
                            printfn "Successfully parsed line %d" nmbr
                            #endif
                            return { id = id; x = x; y = y; z = z; wsp = wsp; h = h; vres = vres }
                    }
                // The first section includes the CSV table definitions which we already know. So skip the first line!
                let resizeArray = ResizeArray(Capacity = endLine - 1 - startLine)
                let init = parseLine >> Option.iter(resizeArray.Add)
                    //|> Option.defaultValue (
                    //    printfn "Data-Parser, WARNING: Parsing line %A with content \"%s\" failed" i (AllLines.[i])
                    //    DataRowFLUMORE.Invalid
                    //)
                Parallel.For(startLine + 1, endLine, init) |> ignore

                resizeArray.ToArray()

            let results = ResizeArray()

            // let subDataSections = ast |> List.filter (function | TimestampSubData _ -> true | _ -> false) |> List.toArray
            let parseSubDataSection =
                function 
                | TimestampSubData (context, _, startLine, endLine) ->
                    let time =
                        match context with
                        | TimestampSubHeader(context, _, _, _) ->
                            match context with 
                            | TimestampHeader(TimestampIdentifier(time, _, _, _), _) ->
                                time.ToString "yyyyMMddHHmmss" // Convert the time to a FME compatible representation, year month day hours minutes seconds
                            | _ -> ""
                        | _ -> 
                            ""
                    let parsingStart = startLine + 1
                    let res = parseCSV parsingStart endLine
                    (res, time)
                    |> Table |> Some
                | _ -> None
              
            ast 
            |> List.toArray 
            |> Array.Parallel.choose parseSubDataSection

            //let rec visitAST remainingAST =
            //    match remainingAST with
            //    | h :: t ->
            //        match h with
            //        | TimestampSubData (context, _, startLine, endLine) ->
            //            let time =
            //                match context with
            //                | TimestampSubHeader(context, _, _, _) ->
            //                    match context with 
            //                    | TimestampHeader(TimestampIdentifier(time, _, _, _), _) ->
            //                        time.ToString "yyyyMMddHHmmss" // Convert the time to a FME compatible representation, year month day hours minutes seconds
            //                    | _ -> ""
            //                | _ -> 
            //                    ""
            //            let parsingStart = startLine + 1
            //            let res = parseCSV parsingStart endLine
            //            (res, time)
            //            |> Table
            //            |> results.Add

            //            printfn "Data-Parser, INFO: Parsing %.2f %% complete." ((float endLine / float AllLines.Length) * 100.)
            //            visitAST t
            //        | _ -> visitAST t
            //    | _ -> results
            //visitAST ast
        | None -> 
            printfn "FATAL: No result of the parser, returned data will be empty!"
            [||]
    stopwatch.Stop()
    let diff = stopwatch.ElapsedMilliseconds
    printfn "Parsing took %d milliseconds." diff
    res

let test () = [|1|]

type Foo = | Bar of int
let foo () = [| Bar 1 |]