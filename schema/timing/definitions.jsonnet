local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timing.definitions";
local s = moo.oschema.schema(ns);

// A temporary schema construction context.
local definitions = {
    fl_cmd_data: s.enum("FixedLengthCommandType", [
        "TimeSync",
        "Echo",
        "SpillStart",
        "SpillStop",
        "RunStart",
        "RunStop",
        "WibCalib",
        "SSPCalib",
        "FakeTrig0",
        "FakeTrig1",
        "FakeTrig2", 
        "FakeTrig3",
        "BeamTrig",
        "NoBeamTrig",
        "ExtFakeTrig",
    ], doc="Fixed length command types")
};

// Output a topologically sorted array.
moo.oschema.sort_select(definitions, ns)
