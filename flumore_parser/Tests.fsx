#r "../packages/NUnit.3.7.1/lib/net45/nunit.framework.dll"
#r "../packages/FsUnit.3.0.0/lib/net45/FsUnit.NUnit.dll"
open FsUnit
open NUnit.Framework
open System.Text


let time = System.DateTime.Now
//let asString = time.ToString("g", System.Globalization.CultureInfo.CreateSpecificCulture("en-us"))

let timeToFMEformat (time:System.DateTime) =
    // YYYYMMDDHHMMSS
    time.ToString "yyyyMMddHHmmss"

timeToFMEformat time