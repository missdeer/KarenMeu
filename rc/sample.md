# 微信公众号格式化工具

网站主页：[https://minidump.info/KarenMeu/](https://minidump.info/KarenMeu/)

> 使用微信公众号编辑器有一个十分头疼的问题——粘贴出来的代码，格式错乱，而且特别丑。这款编辑器能够解决这个问题。

## Features
特性与功能：
- 可自定义Markdown代码编辑主题
- 两种Markdown渲染引擎
- 两种预览模式
- 多种预览主题
- 多种不同代码样式
- 代码长度溢出时横向滚动
- 支持 emoji 表情

## Emoji 表情

目前只有`Lute`引擎支持emoji：

:+1: :-1: :100: :1234: :1st_place_medal: :2nd_place_medal: :3rd_place_medal: :8ball: :a: :ab: :abc: :abcd: :accept: :aerial_tramway: :afghanistan: :airplane: :aland_islands: :alarm_clock: :albania: :alembic: :algeria: :alien: :ambulance: :american_samoa: :amphora: :anchor: :andorra: :angel: :anger: :angola: :angry: :anguilla: :anguished: :ant: :antarctica: :antigua_barbuda: :apple: :aquarius: :argentina: :aries: :armenia: :arrow_backward: :arrow_double_down: :arrow_double_up: :arrow_down: :arrow_down_small: :arrow_forward: :arrow_heading_down: :arrow_heading_up: :arrow_left: :arrow_lower_left: :arrow_lower_right: :arrow_right: :arrow_right_hook: :arrow_up: :arrow_up_down: :arrow_up_small: :arrow_upper_left: :arrow_upper_right: :arrows_clockwise: :arrows_counterclockwise: :art: :articulated_lorry: :artificial_satellite: :aruba: :asterisk: :astonished: :athletic_shoe: :atm: :atom_symbol: :australia: :austria: :avocado: :azerbaijan: 

完整emoji列表参考：https://github.com/88250/lute/blob/master/parse/emoji_map.go

## 二级标题

### 三级标题

#### 四级标题

##### 五级标题

###### 六级标题

## 代码示例
`html`代码
```html
<template>
  <Promised :promise="usersPromise">
    <p slot="pending">Loading...</p>

    <ul slot-scope="users">
      <li v-for="user in users">{{ user.name }}</li>
    </ul>

    <p slot="rejected" slot-scope="error">Error: {{ error.message }}</p>
  </Promised>
</template>
```

`css`代码：
```css
.title {
    color: #000000;
    font-size: 20px;
}
```

`js`代码：
```javascript
function sayHelloWorld (name) {
  console.log('Hello world, ' + name);
}

sayHelloWorld('Frank');
```

`php`代码：

```php
echo 'hello,world'
```

## 表格示例

| 品类 | 个数 | 备注 |
|:-----:|:-----:|:------:|
| 程序猿 | 1   | handsome |
| 程序媛 | 1   | beautiful |


