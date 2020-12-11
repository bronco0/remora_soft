/*
ESP8266 file system builder with PlatformIO support

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// -----------------------------------------------------------------------------
// File system builder
// -----------------------------------------------------------------------------

const gulp = require('gulp');
const htmlmin = require('gulp-htmlmin');
const cleancss = require('gulp-clean-css');
const cssmin = require('gulp-cssmin');
const uglify = require('gulp-uglify');
const gzip = require('gulp-gzip');
const del = require('del');
const inline = require('gulp-inline');
const replace = require('gulp-replace');
const rename = require('gulp-rename');
const pjson = require('./package.json');
const debug = require('gulp-debug');
const concat = require('gulp-concat');

var dest = 'data/';

/* Clean destination folder */
gulp.task('clean', function() {
  return del([dest + '*']);
});

/* Copy static files */
gulp.task('files', function() {
  return gulp.src([
    'webdev/fonts/*.{woff,woff2,ttf}',
    'webdev/favicon.ico'
  ])
  .pipe(gulp.dest(dest + 'fonts'));
});

gulp.task('remora_js', function() {
  return gulp.src([
    'webdev/js/ajaxq.js',
    'webdev/js/autofill.js',
    'webdev/js/validator.js',
    'webdev/js/main.js',
  ])
  .pipe(concat('remora.min.js'))
  //.pipe(uglify())
  .pipe(replace(/@remora_version/, pjson.version))
  .pipe(gulp.dest('webdev/js'));
});

gulp.task('js', ['remora_js'], function() {
  return gulp.src([
    'webdev/js/jquery-2.1.4.min.js',
    'webdev/js/jquery-ui.min.js',
    'webdev/js/bootstrap.min.js',
    'webdev/js/bootstrap-notify.min.js',
    'webdev/js/bootstrap-slider.min.js',
    'webdev/js/bootstrap-table.min.js',
    'webdev/js/bootstrap-table-fr-FR.min.js',
    'webdev/js/remora.min.js'
  ])
  .pipe(concat('remora.js'))
  //.pipe(uglify())
  //.pipe(gzip())
  .pipe(gulp.dest('webdev/js'));
});

gulp.task('gzip_js', ['js'], function() {
  return gulp.src('webdev/js/remora.js')
  .pipe(gzip())
  .pipe(gulp.dest(dest + 'js'));
});

gulp.task('css', function() {
  return gulp.src('webdev/css/*.min.css')
  .pipe(concat('remora.css'))
  .pipe(cleancss({level: {1: {specialComments: 0}}}))
  .pipe(cssmin())
  .pipe(gzip())
  .pipe(gulp.dest(dest + 'css'));
});

/* Process HTML, CSS, JS */
gulp.task('html', function() {
  return gulp.src('webdev/index.htm')
  .pipe(htmlmin({
    collapseWhitespace: true,
    removeComments: true,
    minifyCSS: true,
    minifyJS: false
  }))
  .pipe(gzip())
  .pipe(gulp.dest(dest));
});

/* Build file system */
gulp.task('buildfs', ['clean', 'files', 'html', 'gzip_js', 'css']);
gulp.task('default', ['buildfs']);

// -----------------------------------------------------------------------------
// PlatformIO support
// -----------------------------------------------------------------------------

const spawn = require('child_process').spawn;
const argv = require('yargs').argv;

var platformio = function(target) {
  var args = ['run'];
  if ("e" in argv) { args.push('-e'); args.push(argv.e); }
  if ("p" in argv) { args.push('--upload-port'); args.push(argv.p); }
  if (target) { args.push('-t'); args.push(target); }
  const cmd = spawn('platformio', args);
  cmd.stdout.on('data', function(data) { console.log(data.toString().trim()); });
  cmd.stderr.on('data', function(data) { console.log(data.toString().trim()); });
}

gulp.task('uploadfs', ['buildfs'], function() { platformio('uploadfs'); });
gulp.task('upload', function() { platformio('upload'); });
gulp.task('run', function() { platformio(false); });
