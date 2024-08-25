const std = @import("std");
const genCompileCommands = @import("build/gen_compile_commands.zig").genCompileCommandsFile;

pub const source_files = [_][]const u8{
    "src/mem/allocator.cpp",
    "src/mem/layout.cpp",
};

// TODO: Add flags for release and switch based on that!

fn debugFlags(allocator: std.mem.Allocator, extra_opts: ?[]const []const u8) !std.ArrayList([]const u8) {
    var flags = std.ArrayList([]const u8).initCapacity(allocator, 10) catch unreachable;
    try flags.append("-std=c++20");
    try flags.append("-W");
    try flags.append("-Wall");
    try flags.append("-Werror");
    try flags.append("-Wpedantic");
    try flags.append("-Wstrict-prototypes");
    try flags.append("-Wwrite-strings");
    try flags.append("-Wno-missing-field-initializers");
    try flags.append("-Wno-bit-int-extension");
    try flags.append("-fsanitize=undefined");
    if (extra_opts != null) {
        for (extra_opts.?) |opt| {
            try flags.append(opt);
        }
    }
    return flags;
}

pub fn getSourceFileNames(allocator: std.mem.Allocator, files: []const []const u8) !std.ArrayList([]const u8) {
    var filenames = std.ArrayList([]const u8).init(allocator);
    for (files) |file| {
        var split = std.mem.splitAny(u8, file, "/");
        while (split.next()) |s| {
            if (std.mem.endsWith(u8, s, ".c") or std.mem.endsWith(u8, s, ".cpp")) {
                try filenames.append(s);
            }
        }
    }
    return filenames;
}

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Library
    // ==================================
    const lib = b.addStaticLibrary(.{
        .name = "cbl",
        .target = target,
        .optimize = optimize,
    });
    lib.addIncludePath(b.path("include"));

    // // Define debug macros
    // if (optimize == .Debug) {
    //     lib.root_module.addCMacro("CBL_DEBUG_MODE", "");
    // }

    const filenames = try getSourceFileNames(b.allocator, &source_files);
    for (source_files, 0..) |file, i| {
        const buf = try b.allocator.alloc(u8, file.len + 12);
        defer b.allocator.free(buf);

        // Split filename and only grab the thing before `.c`
        var stream = std.io.fixedBufferStream(buf);
        var writer = stream.writer();
        try writer.print("build/{s}.json", .{filenames.items[i]});
        const out = stream.getWritten();

        const flags = try debugFlags(b.allocator, &.{ "-MJ", out });
        lib.addCSourceFile(.{
            .file = b.path(file),
            .flags = flags.items,
        });
    }
    // lib.linkLibC();
    lib.linkLibCpp();
    b.installArtifact(lib);

    // Generate compile_commands.json
    try genCompileCommands(b);

    // Tests
    // ==================================
    const lib_tests = b.addExecutable(.{
        .name = "cbl_tests",
        .target = target,
        .optimize = optimize,
    });
    lib_tests.addIncludePath(b.path("include"));
    const flags = try debugFlags(b.allocator, null);
    lib_tests.addCSourceFile(.{
        .file = b.path("tests/runner.cpp"),
        .flags = flags.items,
    });
    lib_tests.linkLibCpp();
    lib_tests.linkLibrary(lib);
    const run_lib_unit_tests = b.addRunArtifact(lib_tests);
    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(b.getInstallStep());
    test_step.dependOn(&run_lib_unit_tests.step);
    b.installArtifact(lib_tests);
}
