//open Importer

// Weitere Informationen zu F# unter "http://fsharp.org".
// Weitere Hilfe finden Sie im Projekt "F#-Tutorial".

//let [<Literal>] databaseType = FSharp.Data.Sql.Common.DatabaseProviderTypes.POSTGRESQL
//let [<Literal>] connectionString = "Host=192.168.1.3;Database=Test;Username=postgres;Password=cherry"
//let [<Literal>] resPath = @"C:\Users\Victor\Documents\Development\F\ImportSimulationData"

//let [<Literal>] indivAmount = 1000

//type sql = 
//    FSharp.Data.Sql.SqlDataProvider<databaseType, connectionString, "", resPath, indivAmount, true> 
    
//let ctx = sql.GetDataContext()

//let connection = ctx.CreateConnection()

//[<EntryPoint>]
//[<System.STAThread>]
//let main argv = 
//    printfn "%A" argv
//    let openSimulationFile () =
//        use dialog = new System.Windows.Forms.OpenFileDialog()
//        let res = dialog.ShowDialog()
//        match res with
//        | System.Windows.Forms.DialogResult.OK -> dialog.FileName|> Some
//        | _ -> None

//    let res =
//        match openSimulationFile() with
//        | Some path -> SimulationFileParser.tryParseSimulationFile path |> Some
//        | _ -> None

//    match res with
//    | Some r -> printfn "%A" r
//    | None -> ()
    
//    System.Console.ReadKey() |> ignore
//    0 // Integer-Exitcode zurueckgeben
module NOTHING