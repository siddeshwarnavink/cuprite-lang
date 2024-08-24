output = `./test_mylib --gtest_color=no`

lines = output.split("\n")
has_error = false

lines.each_with_index do |line, index|
  if line.strip.end_with?('Failure')
    has_error = true
    file_path = line.split(':')[0].gsub(Dir.pwd, '')
    file_path.slice!(0)
    line_number =  line.split(':')[1]

    i = index + 1
    err_msg = ""
    while i < lines.length && !lines[i].start_with?('[  FAILED  ]')
      clean_line = lines[i].gsub(/\A\/.+\/\A/, "")
      err_msg << clean_line
      i += 1
    end

    puts "#{file_path}:#{line_number}:\ #{err_msg}"
  end
end

exit 1 if has_error
