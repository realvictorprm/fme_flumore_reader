module Utils
open System.Text.RegularExpressions
    
[<AutoOpen>]
module ActivePatterns =
    let (|Regex|_|) pattern input =
        let m = Regex.Match(input, pattern)
        if m.Success then Some(List.tail [ for g in m.Groups -> g.Value ])
        else None

module Option =
    let someIf predicate value =
        if predicate then Some value else None

module Convert =
    let tryToInt32 i =
        let success, result = System.Int32.TryParse i
        Option.someIf success result
        
    let tryToUInt32 i =
        let success, result = System.UInt32.TryParse i
        Option.someIf success result

    let tryToFloat f =
        let mutable result = 0.;
        let success = System.Double.TryParse(f, System.Globalization.NumberStyles.Float, System.Globalization.NumberFormatInfo(), &result)
        Option.someIf success result
        
        
    let tryToFloat32 f =
        let mutable result = 0.f;
        let success = System.Single.TryParse(f, System.Globalization.NumberStyles.Float, System.Globalization.NumberFormatInfo(), &result)
        Option.someIf success result
        
module Regex =
        
    let isMatch pattern input = Regex.IsMatch(input, pattern)
        
        
    let Match pattern input = Regex.Match(input, pattern)
        
        
    let tryMatch pattern input = 
        let res = Match pattern input
        if res.Success then
            Some res
        else
            None
                
        
    let matchPatternWithOptions pattern options input = Regex.Match(input, pattern, options)
        
        
    let matchPatternWithOptionsAndTimeout pattern options timeout input =
        Regex.Match(input, pattern, options, timeout)
        
        
    let Split pattern input = Regex.Split(input, pattern)
        
        
    let Matches string input = Regex.Matches(input, string)
        
        
    let Replace pattern (replacement:string) input = Regex.Replace(input, pattern, replacement)
    
module Capture = 
    let GetValue (capture:Capture) = capture.Value

module Match =
    let TryGetValue (matchResult:Match) = 
        if matchResult.Success then
            Capture.GetValue matchResult |> Some
        else
            None

module String =

    [<System.Obsolete("param pos changed", true)>]
    let replaceString oldValue (newValue:string) (input:string) =
        input.Replace(oldValue, newValue)
       
    let replaceChar oldChar (newChar:char) (input:string) =
        input.Replace(oldChar, newChar)

module File =
    open System.IO
    let tryReadAllText path =
        if File.Exists path then
            try
                System.IO.File.ReadAllText path
                |> Ok
            with e -> 
#if DEBUG
                printfn "An IO error occured: %A" e
#endif            
                Error (sprintf "%A" e) 
        else
            Error "File doesn't exist!"

    let tryReadAllLines path =
        if File.Exists path then
            try
                System.IO.File.ReadAllLines path
                |> Ok
            with e -> 
#if DEBUG
                printfn "An IO error occured: %A" e
#endif            
                Error (sprintf "%A" e) 
        else
            Error "File doesn't exist!"

module Result =
    let asOption (result:Result<_,_>) =
        match result with
        | Ok value -> Some value
#if DEBUG
        | Error(msg) ->
            printfn "Result message: %A" msg
            None
#else
        | _ -> None
#endif

module DateTime =
    let toUniversalTime (date:System.DateTime) = date.ToUniversalTime()

    let tryParse input =
        let success, res = System.DateTime.TryParse input
        Option.someIf success res


type MaybeBuilder() =
    member this.Bind(x, f) = 
        match x with
        | None -> None
        | Some a -> f a

    member this.Return(x) = 
        Some x

    member this.Zero() =
        None
                        
   
let maybe = new MaybeBuilder()
