Import("env")

def skip_asm(node):
    if "lvgl" in node.get_path():
        return None
    return node

env.AddBuildMiddleware(skip_asm, "*.S")