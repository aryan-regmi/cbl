const std = @import("std");
const source_files = @import("../build.zig").source_files;
const getSourceFileNames = @import("../build.zig").getSourceFileNames;

/// Generate compile_commands.json
pub fn genCompileCommandsFile(b: *std.Build) !void {
    const compile_commands = b.allocator.create(GenerateCompileCommands) catch unreachable;
    compile_commands.* = GenerateCompileCommands.init(b);
    compile_commands.step.dependOn(b.getInstallStep());
    var gen_compile_commands = b.step("lib", "Generates `compile_commands.json`");
    gen_compile_commands.dependOn(&compile_commands.step);
}

const GenerateCompileCommands = struct {
    step: std.Build.Step,

    pub fn init(b: *std.Build) GenerateCompileCommands {
        return GenerateCompileCommands{
            .step = std.Build.Step.init(.{
                .id = .custom,
                .name = "compile_commands",
                .owner = b,
                .makeFn = create_compile_commands,
            }),
        };
    }
};

fn create_compile_commands(s: *std.Build.Step, _: std.Progress.Node) !void {
    const b = s.owner;

    // Final compile_commands.json file to copy into
    const cc_final_path = b.path("build/compile_commands.json").getPath(b);
    var out_stream = try std.fs.createFileAbsolute(cc_final_path, .{});
    defer out_stream.close();

    _ = try out_stream.write("[");
    const filenames = try getSourceFileNames(b.allocator, &source_files);
    for (source_files, 0..) |file, i| {
        // Get databse file for the source file
        const buf = try b.allocator.alloc(u8, file.len + 12);
        defer b.allocator.free(buf);
        var stream = std.io.fixedBufferStream(buf);
        var writer = stream.writer();
        try writer.print("build/{s}.json", .{filenames.items[i]});
        const out = stream.getWritten();

        const cc_raw_path = b.path(out).getPath(b);
        const in_stream = try std.fs.openFileAbsolute(cc_raw_path, .{});
        _ = try out_stream.writeFileAll(in_stream, .{});
        try std.fs.deleteFileAbsolute(cc_raw_path);
    }
    _ = try out_stream.write("]");
}
