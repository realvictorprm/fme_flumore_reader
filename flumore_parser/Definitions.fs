module Definitions
    
type SimulationKind =
    | Prediction
    | Scenario
    
type TimestampKind =
    | Simulation
    | Prediction
    | Scenario

type FileIdentifier = 
    | FileIdentifier of Kind : SimulationKind * Created : System.DateTime * Variant : int32 * Counter : int32

type PackageIdentifier = 
    | PackageIdentifier of 
        Created : System.DateTime * 
        Timesteps : int32 * 
        Variant : int32 * 
        Counter : int32 * 
        From : int32 * 
        To : int32 
        

type TimestampIdentifier = 
    | TimestampIdentifier of PredictionDate : System.DateTime * Kind : TimestampKind * SubSpanCount : int32 * _2DCount : int32

type TimestampSubDataIdentifier = 
    | TimestampSubDataIdentifier of Count:int * ID:int * Version:int

type Deichueberstroemung = 
    | Deichueberstroemung of 
        hm : float *    // Mittl. ueberstroemungshoehe [m] bezogen auf OW
        q : float       // Durchfluß [m³/s]

type Deichbresche = 
    | Deichbresche of 
        bb : float  *   // Breschenbreite [m]
        btm : float *   // Mittl. Breschentiefe [m] bezogen auf OW
        q : float       // Durchfluß [m³/s]

type SituationIdentifier =
    | Ueberstr of Deichueberstroemung
    | Bresche of rw:float * hw:float * Deichbresche
    | Deichentl of rw:float * hw:float * Deichbresche
    | Folgebruch of rw:float * hw:float * Deichbresche
    | InnereEntl of rw:float * hw:float * Deichbresche
    | LinienSM of minkrh:float // minkrh - Minimale Kronenhoehe [m+NN]
    | PunktSM of rw:float * hw:float * maxsh:float // maxsh - Max. Schutzhoehe [m+NN]

type Context =
    | Unattached
    | TimestampSpan of TimestampIdentifier
    | Subspan of SituationIdentifier

type AST =
    | Header of PackageIdentifier
    | TimestampHeader of TimestampIdentifier * At:int
    | TimestampSubHeader of Context:AST * SituationIdentifier * From:int * To:int
    | TimestampSubData of Context:AST * TimestampSubDataIdentifier * From:int * To:int
    | Detached

[<Struct>]
type ParserResult = {
    AllLines : string array
    AST : AST list
}

type DataRowFLUMORE = {
    id : int
    x : float
    y : float
    z : float
    wsp : float
    h : float
    vres : float
} with
    static member Invalid = { id = -1; x = -1.; y = -1.; z = -1.; wsp = -1.; h = -1.; vres = -1. }

type DataTableFLUMORE = | Table of data : DataRowFLUMORE array * time : string