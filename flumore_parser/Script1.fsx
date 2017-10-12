#r "../packages/FSharp.Text.RegexProvider.1.0.0/lib/net40/FSharp.Text.RegexProvider.dll"
#load "Utils.fs"
#load "Definitions.fs"
#load "Patterns.fs"
#load "Parser.fs"

open FSharp.Text

let openSimulationFile () =
    use dialog = new System.Windows.Forms.OpenFileDialog()
    let res = dialog.ShowDialog()
    match res with
    | System.Windows.Forms.DialogResult.OK -> System.IO.Path.GetFullPath dialog.FileName |> Some
    | _ -> None
    
match openSimulationFile() with
| Some path -> Parser.getSimulationFileData path |> ignore
| _ -> ()




